FROM personalroboticsimperial/prl:noetic-cuda117
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
 
# Install this to get libncuvid.so.1
RUN apt install -y libnvidia-decode-470

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
 
WORKDIR /zed
 
# Download and install the ZED SDK
RUN wget -q -O ZED_SDK_Linux_Ubuntu20.run https://download.stereolabs.com/zedsdk/4.1/cu118/ubuntu20 && \
    chmod +x ZED_SDK_Linux_Ubuntu20.run
 
RUN ./ZED_SDK_Linux_Ubuntu20.run --nodiskspace -- silent
 
RUN ln -sf /lib/x86_64-linux-gnu/libusb-1.0.so.0 /usr/lib/x86_64-linux-gnu/libusb-1.0.so  && \
    rm ZED_SDK_Linux_Ubuntu20.run && \
    rm -rf /var/lib/apt/lists/*
 
# Install the ZED Python API
RUN cd /usr/local/zed && \
    wget https://download.stereolabs.com/zedsdk/pyzed -O get_python_api.py && \
    python3 get_python_api.py && \
    python3 -m pip install *.whl && \
    rm *.whl
    
# Fix library paths for USB
# Create symbolic link for libcuda.so.1
# RUN ln -s /usr/local/cuda-11.7/compat/libcuda.so.1 /usr/lib/x86_64-linux-gnu/libcuda.so.1 && \
#     ln -s /usr/local/cuda-11.7/compat/libcuda.so.1 /usr/lib/x86_64-linux-gnu/libcuda.so
 
# Create ZED directory to avoid runtime issues
RUN mkdir -p /root/Documents/ZED/
 
####################################################################################################
###################################### ROS WORKSPACE & Zed Wrapper #####################################
####################################################################################################
 
WORKDIR /root/ros_ws/
RUN mkdir src && source /opt/ros/noetic/setup.bash && catkin init && catkin build
 
 
# RUN DEBIAN_FRONTEND=noninteractive && \
#     apt-get install -y libaria-dev
 
RUN cd src && \
    git clone --recursive https://github.com/stereolabs/zed-ros-wrapper.git && \
    cd ..
 
RUN DEBIAN_FRONTEND=noninteractive apt update
 
#16
RUN apt update && apt install -y ros-noetic-diagnostic-updater ros-noetic-image-transport-plugins && \ 
    . /opt/ros/noetic/setup.bash && \ 
    rosdep update && \ 
    rosdep install --from-paths src --ignore-src -r -y

# RUN . /opt/ros/noetic/setup.sh && \
#    rosdep install --from-paths src --ignore-src -r -y
# RUN DEBIAN_FRONTEND=noninteractive apt update
 
# 17
RUN source /opt/ros/noetic/setup.sh && \
    apt-get update && apt-get install -y libblas-dev liblapack-dev libatlas-base-dev

# 18
RUN catkin config --cmake-args -DCMAKE_CXX_FLAGS="-Wl,--allow-shlib-undefined" && \
    catkin build zed_wrapper
 
# 19
RUN catkin build -DCMAKE_BUILD_TYPE=Release && \
    source ./devel/setup.bash
 
# RUN cd src && git clone --recurse https://github.com/stereolabs/zed-ros-wrapper.git
# RUN apt update && \
#     cd .. && \
#     source devel/setup.bash && DEBIAN_FRONTEND=noninteractive rosdep install --from-paths src --ignore-src -r -y
# RUN python3 -m pip install -U pip && python3 -m pip install opencv-contrib-python
 
# RUN git clone https://github.com/stereolabs/zed-ros-examples.git
# RUN apt update && DEBIAN_FRONTEND=noninteractive rosdep install --from-paths . --ignore-src -r -y
# RUN catkin build -DCMAKE_BUILD_TYPE=Release
 
 
# RUN source devel/setup.bash && \
#     catkin config --cmake-args -DCMAKE_CXX_FLAGS="-Wl,--allow-shlib-undefined" && \
#     catkin build zed_wrapper
 

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

# WORKDIR /root/ros_ws/

# # RUN DEBIAN_FRONTEND=noninteractive && \
# #     apt-get install -y libaria-dev

# RUN cd src && \
#     git clone --recursive https://github.com/stereolabs/zed-ros-wrapper.git && \
#     cd ..

# RUN DEBIAN_FRONTEND=noninteractive apt update
# RUN rosdep install --from-paths src --ignore-src -r -y 

# RUN catkin build -DCMAKE_BUILD_TYPE=Release && \
#     source ./devel/setup.bash