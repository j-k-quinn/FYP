FROM personalroboticsimperial/prl:jetson2004-cuda114-noetic-zed40
SHELL ["/bin/bash", "-c"]
 
# Some useful packages
RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
    python3-catkin-tools \
    git \
    wget \
    lsb-release \
    zstd \
    sudo \
    build-essential \
    cmake \
    python3-pip \
    libpng-dev \
    libgomp1 \
    udev
 
# Upgrade pip and install Python packages
RUN python3 -m pip install numpy opencv-python pyopengl requests
 

# Install YOLOv8
RUN pip install ultralytics

####################################################################################################
##################################### BUILDING ARIA FROM SOURCE  ###################################
####################################################################################################
 
WORKDIR /aria
RUN git clone https://github.com/moshulu/aria-legacy/ && \
    mv aria-legacy/ Aria && \
    cd Aria && \
    make && \
    make install
 
####################################################################################################
############################################# ZED SDK ##############################################
####################################################################################################

# Create a non-root user for SDK installation
RUN useradd -m zeduser && echo "zeduser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Change to that user, download and install SDK
USER zeduser
WORKDIR /home/zeduser

# Download and install the ZED SDK for Jetson
COPY zed_extracted/ /usr/local/zed/

# Switch back to root
USER root

WORKDIR /zed
 
# Install the ZED Python API
RUN cd /usr/local/zed && \
    wget https://download.stereolabs.com/zedsdk/pyzed -O get_python_api.py && \
    python3 get_python_api.py && \
    python3 -m pip install *.whl && \
    rm *.whl
    
# Create ZED directory to avoid runtime issues
RUN mkdir -p /root/Documents/ZED/
 
####################################################################################################
###################################### ROS WORKSPACE & Zed Wrapper #####################################
####################################################################################################
 
WORKDIR /root/ros_ws/
RUN mkdir -p src && source /opt/ros/noetic/setup.bash && catkin init 
 
 
RUN cd src && \
    [ ! -d "zed-ros-wrapper" ] && git clone --recursive https://github.com/stereolabs/zed-ros-wrapper.git || echo "zed-ros-wrapper already exists"

# Cleanly replace the values while preserving indentation
RUN sed -i 's/^\([[:space:]]*publish_tf:\).*/\1 false/' /root/ros_ws/src/zed-ros-wrapper/zed_wrapper/params/common.yaml && \
    sed -i 's/^\([[:space:]]*publish_map_tf:\).*/\1 false/' /root/ros_ws/src/zed-ros-wrapper/zed_wrapper/params/common.yaml

RUN python3 -c "import yaml; yaml.safe_load(open('/root/ros_ws/src/zed-ros-wrapper/zed_wrapper/params/common.yaml'))"

RUN DEBIAN_FRONTEND=noninteractive apt update
 

RUN apt update && apt install -y ros-noetic-diagnostic-updater ros-noetic-image-transport-plugins && \ 
    . /opt/ros/noetic/setup.bash && \ 
    rosdep update && \ 
    rosdep install --from-paths src --ignore-src -r -y


RUN source /opt/ros/noetic/setup.sh && \
    apt-get update && apt-get install -y libblas-dev liblapack-dev libatlas-base-dev


RUN catkin config --cmake-args -DCMAKE_CXX_FLAGS="-Wl,--allow-shlib-undefined" && \
    catkin build zed_wrapper
 

RUN catkin build -DCMAKE_BUILD_TYPE=Release && \
    source ./devel/setup.bash
 


####################################################################################################
###################################### ROS WORKSPACE & ROSARIA #####################################
#################################################################################################### 
 
WORKDIR /root/ros_ws/
 
# Clone the rosaria package
RUN cd src && \
    git clone https://github.com/amor-ros-pkg/rosaria.git
#   git clone --recursive https://github.com/stereolabs/zed-ros-wrapper.git
 
 
# Install necessary ROS dependencies
RUN source /opt/ros/noetic/setup.bash && \
    DEBIAN_FRONTEND=noninteractive apt update && \
        apt install -y --no-install-recommends \
        ros-noetic-tf
 
# Install additional ROS tools
RUN apt-get update && \
    apt-get install -y \
    ros-noetic-rviz \
    ros-noetic-joy

# Install TF2 tools needed for static_transform_publisher
RUN apt-get update && \
    apt-get install -y \
    ros-noetic-tf2-ros \
    ros-noetic-tf2-tools
 

# Build the ROS workspace
RUN source /opt/ros/noetic/setup.bash && catkin build


####################################################################################################
############################################# ROS LiDAR ############################################
#################################################################################################### 
 
RUN cd src && \
    git clone https://github.com/robopeak/rplidar_ros.git 

# Modify the serial port in the launch file 
#RUN sed -i 's|<param name="serial_port" type="string" value="/dev/ttyUSB0"/>|<param name="serial_port" type="string" value="/dev/ttyUSB2"/>|' /root/ros_ws/src/rplidar_ros/launch/rplidar.launch

RUN source /opt/ros/noetic/setup.bash && catkin build

####################################################################################################
######################################## Pioneer P3-DX Model #######################################
#################################################################################################### 

RUN apt-get update && apt-get install -y \
    ros-noetic-controller-manager \
    && rm -rf /var/lib/apt/lists/*

# Ensure ROS package sources are available 
RUN echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" | tee /etc/apt/sources.list.d/ros-latest.list \ 
    && apt-get update && apt-get install -y ros-noetic-gazebo-ros-pkgs

RUN cd src && \
    git clone https://github.com/mario-serna/pioneer_p3dx_model.git 

RUN catkin build

CMD /bin/bash
#CMD source devel/setup.bash && rosrun rosaria RosAria _port:=${P3AT_USB_PORT} 


####################################################################################################
########################################### Controller.py ##########################################
#################################################################################################### 

RUN cd src/rosaria && \
    mkdir scripts

# Copy scripts from the host into the container
COPY scripts /root/ros_ws/src/rosaria/scripts

RUN chmod +x /root/ros_ws/src/rosaria/scripts/*.py
# RUN chmod +x /root/ros_ws/src/rosaria/scripts/listener.py

# Modify CMakeLists.txt to include the custom scripts 
RUN echo 'catkin_install_python(PROGRAMS scripts/publisher.py scripts/subscriber.py scripts/controller.py' >> \ 
    /root/ros_ws/src/rosaria/CMakeLists.txt && \ 
    echo ' DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})' >> \ 
    /root/ros_ws/src/rosaria/CMakeLists.txt

COPY full_system_pkg/full_system /root/ros_ws/src/full_system

# Rebuild to include full_system package
WORKDIR /root/ros_ws
RUN source /opt/ros/noetic/setup.bash && catkin build full_system

RUN source /opt/ros/noetic/setup.bash && \
    source devel/setup.bash && \
    rospack list | grep full_system
