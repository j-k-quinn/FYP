#!/usr/bin/env python3
import rospy
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
from std_msgs.msg import Bool

emergency_stop= False

 
# Scaling factors for linear and angular movement
l_scale = 1.0  # Linear speed scaling factor
a_scale = 1.0  # Angular speed scaling factor
 
# Joystick subscriber callback
def joy_callback(data):
    global emergency_stop

    # Emergency stop toggle (example: button 6 = Back)
    if data.buttons[1] == 1:  # Button pressed
        emergency_stop = True
    else:
        emergency_stop = False

    twist = Twist()
 
    if emergency_stop:
        twist.linear.x = 0
        twist.linear.y = 0
        twist.angular.z = 0
        rospy.logwarn_throttle(1, "Emergency STOP active!")

    else:
        # Use axes for analog stick values
        # axes[7]: up/down → forward/backward
        # axes[6]: left/right → left/right turn
    
        twist.linear.x = l_scale * data.axes[7]
        twist.angular.z = a_scale * data.axes[6]  # Negative for right-positive angular direction

    rospy.loginfo("Twist command: linear.x = %.2f, angular.z = %.2f", twist.linear.x, twist.angular.z)
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
