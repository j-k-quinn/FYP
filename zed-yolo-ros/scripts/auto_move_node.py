#!/usr/bin/env python3
 
import rospy
from geometry_msgs.msg import Twist
import time
import math
import socket
import std_msgs.msg
from std_msgs.msg import Float32MultiArray
 
# Target coordinate (in meters)
TARGET_X = -0.1
TARGET_Y = 0.0
 
SPEED = 0.2         # Linear speed (m/s)
ROTATE_SPEED = 0.5  # Angular speed (rad/s)
 
confirmation_pub = None

def rotate(pub, angle_rad):
    twist = Twist()
    duration = abs(angle_rad / ROTATE_SPEED) * 1.08 # 1.1
    twist.angular.z = ROTATE_SPEED if angle_rad > 0 else -ROTATE_SPEED
 
    rospy.loginfo(f"Rotating {math.degrees(angle_rad):.2f} degrees.")
    start_time = time.time()
    while time.time() - start_time < duration and not rospy.is_shutdown():
        pub.publish(twist)
        rospy.sleep(0.1)
 
    twist.angular.z = 0.0
    pub.publish(twist)
    rospy.loginfo("Rotation complete.")
 
def move_forward(pub, distance, reverse=False):
    twist = Twist()
    duration = abs(distance / SPEED) * 1.07 # 1.12
    direction = -SPEED if reverse else SPEED
    twist.linear.x = direction
 
    direction_text = "backward" if reverse else "forward"
    rospy.loginfo(f"Moving {direction_text} {distance:.2f} meters.")
    start_time = time.time()
    while time.time() - start_time < duration and not rospy.is_shutdown():
        pub.publish(twist)
        rospy.sleep(0.1)
 
    twist.linear.x = 0.0
    pub.publish(twist)
    rospy.loginfo("Linear movement complete.")

def get_pending_id():
    try:
        with open("/tmp/pending_id.txt", "r") as f:
            return int(f.read().strip())
    except Exception:
        rospy.logwarn("Could not read pending object ID. Defaulting to -1.")
        return -1

def callback(msg):
    data = msg.data
    if len(data) == 4:
        x, y, z, width = data
    elif len(data) == 3:
        x, y, z = data
        width = 5.5  # fallback default
    else:
        rospy.logwarn(f"[MOVEMENT] Invalid target format: expected 3 or 4 values, got {len(data)}")
        return

    rospy.loginfo(f"[MOVEMENT] Received target: x={x}, y={y}")
    angle_rad = math.atan2(y, x)
    distance = math.hypot(x, y)
    reverse = x < 0
    if reverse:
        angle_rad += math.pi
        angle_rad = (angle_rad + math.pi) % (2 * math.pi) - math.pi
    rotate(pub, angle_rad)
    move_forward(pub, distance, reverse=reverse)

    object_id = get_pending_id()

    # Signal gripper after move
    if send_to_gripper(z, width, object_id):
        return_to_saved_position(pub, x, y)
    else:
        rospy.logwarn("Skipping return because grip confirmation failed.")
 
def send_to_gripper(z, width, object_id):
    import socket
    from std_msgs.msg import Int32

    global confirmation_pub

    HOST = "10.0.1.27"  # Your Pi's IP
    PORT = 5005

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(f"0.0,0.0,{z},{width}\n".encode())  # Now sending 4 values
            rospy.loginfo(f"[MOVEMENT] Sent z={z:.2f}, width={width:.2f} to gripper")

            response = s.recv(1024).decode().strip()
            if response == "grip_done":
                rospy.loginfo("[MOVEMENT] Received grip confirmation from Pi.")

                # Send confirmation to detection node
                if confirmation_pub:
                    msg = Int32()
                    msg.data = object_id
                    confirmation_pub.publish(msg)
                    rospy.loginfo(f"[CONFIRMATION] Published ID {object_id} to /grip_confirmation")

                return True
            else:
                rospy.logwarn(f"[MOVEMENT] Unexpected response: {response}")
                return False

    except Exception as e:
        rospy.logwarn(f"[MOVEMENT] Failed to send z and width to Pi: {e}")
        return False


def send_grip_confirmation(object_id):
    global confirmation_pub
    if confirmation_pub:
        msg = std_msgs.msg.Int32()
        msg.data = object_id
        confirmation_pub.publish(msg)
        rospy.loginfo(f"[CONFIRMATION] Sent grip confirmation for ID {object_id}")

def return_to_saved_position(pub, x, y):
    distance = math.hypot(x, y) * 0.5

    rospy.loginfo(f"[RETURN] Moving backward {distance:.2f} meters to return to original spot.")
    move_forward(pub, distance, reverse=True)

    rospy.loginfo("[RETURN] Return complete.")

def main():
    global pub, confirmation_pub
    rospy.init_node("go_to_point")
    pub = rospy.Publisher('/RosAria/cmd_vel', Twist, queue_size=10)
    confirmation_pub = rospy.Publisher("/grip_confirmation", std_msgs.msg.Int32, queue_size=1)
 
    # Subscribe to object target
    rospy.Subscriber("/object_target", Float32MultiArray, callback)
 
    rospy.loginfo("Movement node ready, waiting for object target...")
 
    rospy.spin()  # Keep the node alive and let callback handle movement
    
if __name__ == "__main__":
    main()
