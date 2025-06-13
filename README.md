# Shared Control of Assistive Robots Using Computer Vision

## Hardware Prerequisites

Ensure the following steps are completed before starting:

- **Charge the PeopleBot batteries** – Verify by checking the 12V output on each battery.  
- **Charge the Anker Solix C300 DC Portable Power Station** – Use the ON button to check the battery level on the LED display.  
- **Verify that all hardware is connected and powered**:
  - PeopleBot  
  - ZED 2i camera  
  - RPLIDAR  
  - Xbox Adaptive Controller  
  - All connected via USB to the Jetson Orin AGX  
- **Power Supply**:
  - Jetson: powered via USB-C from the Anker Solix C300  
  - Raspberry Pi: powered via USB from the Anker  
- **Power On**:
  - Flip ON switch on PeopleBot  
  - Turn on the Anker power station  
- **Check serial ports**:
  - PeopleBot: `/dev/ttyUSB0` and `/dev/ttyUSB1`  
  - RPLIDAR: `/dev/ttyUSB2`  

---


# Getting started

These instructions assume you're launching multiple terminal windows.

## First Terminal Window
```bash
ssh johannaquinn@10.0.1.27	# Password written on the Raspberry Pi case.

# If the gripper is at the starting position run:
python Documents/gripper_socket_server.py
```

If the gripper isn't in the starting position (fully up and fully open) then you can manually adjust it by running this script and changing it based on your demands:
```bash
python Documents/gripper_wrapper.py
```

## Second Terminal Window
> **NB**: In order to access the right folder on the Jetson, connect to monitor through a DisplayPort cable. Then go to files -> go to other locations -> go to 1.0TB Volume -> Press “Open in terminal” -> now you should be able to ssh into the right folder.
```bash
ssh prl@10.0.0.59	# Password written on the PeopleBot under the Jetson.

cd ../..

# Access the right folder on the Jetson
cd media/prl/3923b349-507f-4b3e-b2ad-4b5a3d3f6d6c/FYP

#Run the Docker image
make run
```
If successful you should get the following output prompting you to type the password for the PeopleBot: 
```bash
p3at_ros_driver
p3at_ros_driver
[sudo] password for prl:
```
> **NB**: The password is found on the physical PeopleBot's top plate

You should then get this output:
```bash
Error response from daemon: No such container: p3at_ros_driver
Error response from daemon: No such container: p3at_ros_driver
if [ -n "$DISPLAY" ]; then xhost +si:localuser:root > /dev/null; fi
docker run \
--gpus all \
--runtime=nvidia \
-e NVIDIA_VISIBLE_DEVICES=all \
-e NVIDIA_DRIVER_CAPABILITIES=all \
-e DISPLAY= \
-e QT_X11_NO_MITSHM=1 \
-e P3AT_USB_PORT="/dev/ttyUSB1" \
-v /dev:/dev \
-v ./rplidar_ros:/root/ros_ws/src/rplidar_ros \
-v ./full_system_pkg/full_system:/root/ros_ws/src/full_system \
-v ./p3dx_description:/root/ros_ws/src/pioneer_p3dx_model/p3dx_description \
-v ./scripts:/root/ros_ws/src/scripts \
-v ./zed-yolo-ros:/root/ros_ws/src/zed-yolo-ros \
--privileged \
--network host \
--name p3at_ros_driver \
-it \
prl_p3at_ros_driver:latest
```

Once in the image enter `source devel/setup.bash` and then start `roscore`.

##  Third Terminal Window
In all new terminals run:
```bash
docker exec -it p3at_ros_driver bash

# Then initialise ROS
source devel/setup.bash
```

To connect to the PeopleBot run:
```bash
rosrun rosaria RosAria
```

If everything goes well, you should see something like:
```bash
[ INFO] [1749559152.229945039]: RosAria: set port: [/dev/ttyUSB0]
Connnecting to robot using TCP connection to localhost:8101...
Could not connect to simulator, connecting to robot through serial port /dev/ttyUSB0.
Syncing 0
Syncing 1
Syncing 2
Connected to robot.
Name: PeopleBot
Type: Pioneer
Subtype: peoplebot-sh
ArConfig: Config version: 2.0
Loaded robot parameters from /usr/local/Aria/params/peoplebot-sh.p
Robot Serial Number: Serial
ArRobotPacketReceiver::receivePacket: bad packet, bad checksum
ArRobotConnector: Connecting to MTX batteries (if neccesary)...
ArRobotConnector: Connecting to MTX sonar (if neccesary)...
[ INFO] [1749559153.074780622]: This robot's TicksMM parameter: 128
[ INFO] [1749559153.075395025]: This robot's DriftFactor parameter: 0
[ INFO] [1749559153.075967508]: This robot's RevCount parameter: 16815
[ INFO] [1749559153.114006093]: rosaria: Setup complete
[ INFO] [1749559153.150201789]: RosAria: publishing new recharge state 0.
[ INFO] [1749559153.150280862]: RosAria: publishing new motors state 0.
[ INFO] [1749559154.747316776]: RosAria: publishing new motors state 1.
```

## Fourth Terminal Window
To launch the controller, LiDAR sensor and calibration, run:
```bash
roslaunch full_system full_system.launch
```

## Fifth Terminal Window
To run the object detection script run:
```bash
python src/zed-yolo-ros/scripts/auto_move_node.py
```
Which should give the following output:
```bash
[INFO] [1749559009.012022]: Movement node ready, waiting for object target...
```

## Final Terminal Window
To run the object detection script run with a specified Scenario:
> **NB**: The default Scenario is set to 1, which is the single object automated grab task
```bash
python src/zed-yolo-ros/scripts/detector.py --scenario=6	# Change the Scenario number to test them individually to review results from the Final Report
```
