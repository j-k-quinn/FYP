#!/usr/bin/env python3
import rospy
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
 
# Scaling factors for linear and angular movement
l_scale = 1.0  # Linear speed scaling factor
a_scale = 1.0  # Angular speed scaling factor
 
# Joystick subscriber callback
def joy_callback(data):
    # Create Twist message to publish
    twist = Twist()
 
    # Axis mappings: joystick stick movements
    twist.linear.x = l_scale * data.buttons[1]  # Left stick vertical for linear movement (forward/backward)
    twist.angular.z = a_scale * data.buttons[0]  # Left stick horizontal for angular movement (turning)
 
    # Publish the velocity command based on joystick input
    rospy.loginfo("Publishing twist: linear.x = %f, angular.z = %f", twist.linear.x, twist.angular.z)
    pub.publish(twist)
 
def start():
    # Publisher to control the robot
    global pub
    pub = rospy.Publisher('RosAria/cmd_vel', Twist)
 
    # Subscribe to the joy topic for joystick input
    rospy.Subscriber("joy", Joy, joy_callback)
 
    # Initialize the ROS node
    rospy.init_node('Joy2RobotControl')
    # Keep the node running and processing callbacks
    rospy.loginfo("Joystick to robot control node initialized.")
    rospy.spin()
 
if __name__ == '__main__':
    start()
