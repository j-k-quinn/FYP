#!/usr/bin/env python3

import rospy
import zed_interfaces.msg as zed_msgs

import numpy as np
import argparse
import torch
import cv2
import pyzed.sl as sl
from ultralytics import YOLO

from threading import Lock, Thread
from time import sleep
import shutil
import os

import tf2_ros
import tf2_geometry_msgs
import geometry_msgs.msg
from geometry_msgs.msg import PointStamped
from tf.transformations import quaternion_from_euler

processed_ids = set()
lock = Lock()
run_signal = False
exit_signal = False
class_names = []

CAMERA_NAME = "zed2i"

def xywh2abcd(xywh):
    output = np.zeros((4, 2))
    x_min = (xywh[0] - 0.5*xywh[2])
    x_max = (xywh[0] + 0.5*xywh[2])
    y_min = (xywh[1] - 0.5*xywh[3])
    y_max = (xywh[1] + 0.5*xywh[3])
    output[0] = [x_min, y_min]
    output[1] = [x_max, y_min]
    output[2] = [x_min, y_max]
    output[3] = [x_max, y_max]
    return output

def detections_to_custom_box(detections):
    output = []
    for det in detections:
        xywh = det.xywh[0]
        obj = sl.CustomBoxObjectData()
        obj.bounding_box_2d = xywh2abcd(xywh)
        obj.label = det.cls
        obj.probability = det.conf
        obj.is_grounded = False
        output.append(obj)
    return output

def torch_thread(model_name, img_size, conf_thres=0.2, iou_thres=0.45):
    global image_net, exit_signal, run_signal, detections, class_names

    print("Intializing Network...")
    script_path = os.path.dirname(os.path.realpath(__file__))
    models_path = script_path+'/../models/'
    model_path = models_path+model_name+'.pt'
    model_labels_path = models_path+model_name+'_labels.txt'

    if not os.path.isfile(model_path):
        print("Model not found, downloading it...")
        model = YOLO(model_name+'.pt')
        class_names = model.names
        shutil.copy(model_name+'.pt', models_path+model_name+'.pt')
        with open(model_labels_path, 'w') as f:
            for i in range(len(class_names)):
                f.write("%d, %s\n" % (i, class_names[i]))

    model = YOLO(models_path+model_name+'.pt')
    with open(model_labels_path, 'r') as f:
        class_names = [line.split(', ')[1] for line in f.read().splitlines()]

    while not exit_signal:
        if run_signal:
            lock.acquire()
            img = cv2.cvtColor(image_net, cv2.COLOR_BGRA2RGB)
            det = model.predict(img, save=False, imgsz=img_size, conf=conf_thres, iou=iou_thres)[0].cpu().numpy().boxes
            detections = detections_to_custom_box(det)
            lock.release()
            run_signal = False
        sleep(0.001)

def ros_wrapper(objects):
    ros_msg = zed_msgs.ObjectsStamped()
    ros_msg.header.stamp = rospy.Time.now()
    ros_msg.header.frame_id = CAMERA_NAME + "_left_camera_frame"
    obj_list = []
    for obj in objects.object_list:
        obj_msg = zed_msgs.Object()
        obj_msg.label = class_names[obj.raw_label]
        obj_msg.label_id = obj.raw_label
        obj_msg.sublabel = repr(obj.id)
        obj_msg.instance_id = obj.id
        obj_msg.confidence = obj.confidence
        obj_msg.position = list(obj.position)
        obj_msg.position_covariance = list(obj.position_covariance)
        obj_msg.velocity = list(obj.velocity)
        obj_msg.tracking_available = True
        obj_msg.tracking_state = {"OFF": 0, "OK": 1}.get(repr(obj.tracking_state), 2)
        if len(obj.bounding_box) == 8:
            for i in range(8):
                obj_msg.bounding_box_3d.corners[i].kp = list(obj.bounding_box[i])
        obj_list.append(obj_msg)
    ros_msg.objects = obj_list
    return ros_msg

def local_to_base_link_transform(msg, tfBuffer):
    try:
        if tfBuffer.can_transform("base_link", CAMERA_NAME + "_left_camera_frame", rospy.Time(0), rospy.Duration(2.0)):
            transform = tfBuffer.lookup_transform("base_link", CAMERA_NAME + "_left_camera_frame", rospy.Time(0), rospy.Duration(1.0))
            for obj in msg.objects:
                p = PointStamped()
                p.header.stamp = rospy.Time(0)
                p.header.frame_id = CAMERA_NAME + "_left_camera_frame"
                p.point.x, p.point.y, p.point.z = obj.position
                obj.position = tf2_geometry_msgs.do_transform_point(p, transform).point
                obj.position = [obj.position.x, obj.position.y, obj.position.z]
                rospy.loginfo(f"Detected {obj.label} at position (x={obj.position[0]:.2f}, y={obj.position[1]:.2f}, z={obj.position[2]:.2f}) in base_link frame")
                # start of new code
                TARGET_LABEL = "bottle"  
                if obj.label == TARGET_LABEL and obj.instance_id not in processed_ids:
                    x, y, z = obj.position
                    try:
                        # Send z (or all coords) to the Raspberry Pi
                        import socket
                        PI_IP = "10.0.1.27" 
                        PI_PORT = 5005
 
                        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                            s.connect((PI_IP, PI_PORT))
                            message = f"{x:.3f},{y:.3f},{z:.3f}"
                            s.sendall(message.encode('utf-8'))
                            rospy.loginfo(f"Sent position to Pi: {message}")
                        
                        processed_ids.add(obj.instance_id)

                    except Exception as e:
                        rospy.logwarn(f"Failed to send position to Pi: {e}")
                # end of new code
                    
                if obj.bounding_box_3d and len(obj.bounding_box_3d.corners) == 8:
                    bb = []
                    for corner in obj.bounding_box_3d.corners:
                        p.point.x, p.point.y, p.point.z = corner.kp
                        corner.kp = tf2_geometry_msgs.do_transform_point(p, transform).point
                        corner.kp = [corner.kp.x, corner.kp.y, corner.kp.z]
                        bb.append(corner.kp)
                    # rospy.loginfo(f"Bounding box corners in base_link: {bb}")
            msg.header.frame_id = "base_link"
        else:
            rospy.logwarn("Transform not available yet between base_link and zed2i_left_camera_frame")
    except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
        rospy.logwarn(f"Transform lookup failed: {e}")
    return msg
 
def main():
    global image_net, exit_signal, run_signal, detections, class_names
 
    rospy.init_node("zed_yolo_ros", anonymous=False)
    rospy.loginfo("ZED YOLO node started")
 
    pub_l = rospy.Publisher(CAMERA_NAME+'/od_yolo', zed_msgs.ObjectsStamped, queue_size=50)
    pub_g = rospy.Publisher(CAMERA_NAME+'/od_yolo_base_link_frame', zed_msgs.ObjectsStamped, queue_size=50)
 
    tfBuffer = tf2_ros.Buffer()
    listener = tf2_ros.TransformListener(tfBuffer)
    static_broadcaster = tf2_ros.StaticTransformBroadcaster()
 
    def publish_tf(parent, child, x=0.0, y=0.0, z=0.0, roll=0.0, pitch=0.0, yaw=0.0):
        t = geometry_msgs.msg.TransformStamped()
        t.header.stamp = rospy.Time.now()
        t.header.frame_id = parent
        t.child_frame_id = child
        t.transform.translation.x = x
        t.transform.translation.y = y
        t.transform.translation.z = z
        q = quaternion_from_euler(roll, pitch, yaw)
        t.transform.rotation.x = q[0]
        t.transform.rotation.y = q[1]
        t.transform.rotation.z = q[2]
        t.transform.rotation.w = q[3]
        static_broadcaster.sendTransform(t)
 
    publish_tf("base_link", "camera_base_link", 0.05, 0.0, 1.50)
    rospy.loginfo("Published base_link -> camera_base_link")
 
    rospy.sleep(0.5)
 
    publish_tf("camera_base_link", "zed2i_base_link")
    publish_tf("zed2i_base_link", "zed2i_camera_center")
    publish_tf("zed2i_camera_center", "zed2i_left_camera_frame", 0.0, -0.01, 0.0)
 
    rospy.sleep(0.5)
 
    capture_thread = Thread(target=torch_thread, kwargs={
        'model_name': opt.model_name,
        'img_size': opt.img_size,
        "conf_thres": opt.conf_thres
    })
    capture_thread.start()
 
    print("Initializing Camera...")
    zed = sl.Camera()
 
    input_type = sl.InputType()
    if opt.svo is not None:
        input_type.set_from_svo_file(opt.svo)
 
    init_params = sl.InitParameters(input_t=input_type, svo_real_time_mode=True)
    init_params.coordinate_units = sl.UNIT.METER
    init_params.depth_mode = sl.DEPTH_MODE.ULTRA
    init_params.coordinate_system = sl.COORDINATE_SYSTEM.RIGHT_HANDED_Z_UP_X_FWD
    init_params.depth_maximum_distance = 50
 
    runtime_params = sl.RuntimeParameters()
    status = zed.open(init_params)
 
    if status != sl.ERROR_CODE.SUCCESS:
        print(repr(status))
        return
 
    image_left_tmp = sl.Mat()
    print("Initialized Camera")
 
    positional_tracking_parameters = sl.PositionalTrackingParameters()
    zed.enable_positional_tracking(positional_tracking_parameters)
 
    obj_param = sl.ObjectDetectionParameters()
    obj_param.detection_model = sl.OBJECT_DETECTION_MODEL.CUSTOM_BOX_OBJECTS
    obj_param.enable_tracking = True
    zed.enable_object_detection(obj_param)
 
    objects = sl.Objects()
    obj_runtime_param = sl.ObjectDetectionRuntimeParameters()
 
    try:
        while not rospy.is_shutdown():
            if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
                lock.acquire()
                zed.retrieve_image(image_left_tmp, sl.VIEW.LEFT)
                image_net = image_left_tmp.get_data()
                lock.release()
                run_signal = True
 
                while run_signal:
                    sleep(0.001)
 
                lock.acquire()
                zed.ingest_custom_box_objects(detections)
                lock.release()
                zed.retrieve_objects(objects, obj_runtime_param)
 
                ros_msg = ros_wrapper(objects)
                # rospy.loginfo_throttle(5, tfBuffer.all_frames_as_string())
                pub_l.publish(ros_msg)
                pub_g.publish(local_to_base_link_transform(ros_msg, tfBuffer))
    except KeyboardInterrupt:
        rospy.loginfo("Shutdown signal received (Ctrl+C)")
    finally:
        exit_signal = True
        capture_thread.join()
        zed.close()
        rospy.loginfo("Node shut down")
 
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_name', type=str, default='yolo11l', help='model path(s)')
    parser.add_argument('--svo', type=str, default=None, help='optional svo file')
    parser.add_argument('--img_size', type=int, default=416, help='inference size (pixels)')
    parser.add_argument('--conf_thres', type=float, default=0.4, help='object confidence threshold')
    opt = parser.parse_args()
 
    with torch.no_grad():
        main()