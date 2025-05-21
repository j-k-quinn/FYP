#!/usr/bin/env python3
 
import rospy
import geometry_msgs.msg
import tf2_ros
from tf.transformations import quaternion_from_euler
 
def main():
    rospy.init_node("static_tf_test")
 
    broadcaster = tf2_ros.StaticTransformBroadcaster()
    t = geometry_msgs.msg.TransformStamped()
    t.header.stamp = rospy.Time(0)
    t.header.frame_id = "base_link"
    t.child_frame_id = "camera_base_link"
    t.transform.translation.x = 0.05
    t.transform.translation.y = 0.0
    t.transform.translation.z = 1.10
 
    q = quaternion_from_euler(0, 0, 0)
    t.transform.rotation.x = q[0]
    t.transform.rotation.y = q[1]
    t.transform.rotation.z = q[2]
    t.transform.rotation.w = q[3]
 
    broadcaster.sendTransform(t)
    rospy.loginfo("Published base_link → camera_base_link transform.")
 
    rospy.spin()
 
if __name__ == "__main__":
    main()