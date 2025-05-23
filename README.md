# Shared Control of Assistive Robots Using Computer Vision


# Prerequisites

## Theory
Some basic notions of Docker and ROS would be really helpful to get you started. You will learn more about those tools during the class, but you should try to get a basic understanding of the following notions before continuing:
- Linux
  - Environment variables

- Docker
  - Container
  - Image
  - Volumes
  - `docker build`, `docker run`, `docker exec` commands

- ROS
  - Workspace
  - Package
  - Node
  - Topic
  - `roscore`, `roslaunch`, `rosrun`

## Software
From a technical standpoint, you need to install the following on your (Linux) computer: [docker](https://docs.docker.com/engine/install/), [git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git).

You also need to [know your IP](https://opensource.com/article/18/5/how-find-ip-address-linux).

Here is a short installation example on Ubuntu:

```bash
sudo apt install git -y
curl https://get.docker.com | sh && sudo systemctl --now enable docker
```

## Hardware
All P3-ATs have beem checked by the GTAs, so you robot should be working. All you need to do is:
 - **Make sure the batteries are charged** - check the 12V output on each battery
 - **Check that you are plugged in the right serial port** - the one near the motors, reset, etc buttons (see picture below)
 - Check that the other end of your serial to USB adaptor is **plugged in your computer**


# Getting started

> **NB**: Docker requires sudo access, so you will have to use `sudo XXX` and enter your password often. To avoid this, type `sudo usermod -aG docker [YOUR_USERNAME]`, and then reboot your computer. You can then replace `sudo make XXX` by `make XXX` in all the following.
```bash
# Clone the repo
git clone -b jetson-object-detection --single-branch https://github.com/j-k-quinn/FYP.git

cd FYP

# Build the Docker image containing the driver
sudo make build

#Run the Docker image
sudo make run
```
Once in the image enter `source devel/setup.bash` and then start `roscore`.

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
[ INFO] [1638375581.966504576]: RosAria: set port: [/dev/ttyUSB0]
Connnecting to robot using TCP connection to localhost:8101...
Could not connect to simulator, connecting to robot through serial port /dev/ttyUSB0.
Syncing 0
Syncing 1
Syncing 2
Connected to robot.
Name: Imperial_3097
Type: Pioneer
Subtype: p3at-sh
ArConfig: Config version: 2.0
Loaded robot parameters from /usr/local/Aria/params/p3at-sh.p
Robot Serial Number: GJCC3097
ArRobotConnector: Connecting to MTX batteries (if neccesary)...
ArRobotConnector: Connecting to MTX sonar (if neccesary)...
[ INFO] [1638375582.790797790]: This robot's TicksMM parameter: 138
[ INFO] [1638375582.791634567]: This robot's DriftFactor parameter: 0
[ INFO] [1638375582.792650097]: This robot's RevCount parameter: 32550
[ INFO] [1638375582.822727512]: rosaria: Setup complete
```

To launch the sensors and controller run:
```bash
roslaunch full_system full_system.launch
```

To run the object detection script run:
```bash
roslaunch full_system full_system.launch
```
