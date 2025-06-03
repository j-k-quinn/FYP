#!/usr/bin/env python3

import rospy
import zed_interfaces.msg as zed_msgs

import numpy as np

import argparse
import torch
import cv2
import pyzed.sl as sl
import time
from ultralytics import YOLO

from threading import Lock, Thread
from time import sleep

import shutil
import os

import tf2_ros
import tf2_geometry_msgs
from geometry_msgs.msg import PointStamped
from std_msgs.msg import Float32MultiArray
from sensor_msgs.msg import Joy
from sensor_msgs.msg import LaserScan
import math

lock = Lock()
run_signal = False
exit_signal = False
class_names = []

CAMERA_NAME = "zed2i"
processed_ids = set()

joystick_state = {
    "left_pressed": False,
    "right_pressed": False,
    "last_offset": 0
}

selection_state = {
    "active": False,
    "last_update_time": 0,
    "selected_idx": 0
}

latest_scan = None

def scan_callback(scan_msg):
    global latest_scan
    latest_scan = scan_msg

def xywh2abcd(xywh):
    output = np.zeros((4, 2))

    # Center / Width / Height -> BBox corners coordinates
    x_min = (xywh[0] - 0.5*xywh[2]) #* im_shape[1]
    x_max = (xywh[0] + 0.5*xywh[2]) #* im_shape[1]
    y_min = (xywh[1] - 0.5*xywh[3]) #* im_shape[0]
    y_max = (xywh[1] + 0.5*xywh[3]) #* im_shape[0]

    # A ------ B
    # | Object |
    # D ------ C

    output[0][0] = x_min
    output[0][1] = y_min

    output[1][0] = x_max
    output[1][1] = y_min

    output[2][0] = x_min
    output[2][1] = y_max

    output[3][0] = x_max
    output[3][1] = y_max
    return output

def detections_to_custom_box(detections):
    output = []
    for i, det in enumerate(detections):
        xywh = det.xywh[0]

        # Creating ingestable objects for the ZED SDK
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

    # Check if the model does not exists
    if not os.path.isfile(model_path):
        print("Model not found, downloading it...")

        # Get the PyTorch model
        model = YOLO(model_name+'.pt')
        class_names = model.names

        # Export the model as an engine
        #model.export(format='engine')    

        # Copy the models into the correct directory
        shutil.copy(model_name+'.pt', models_path+model_name+'.pt')
        #shutil.copy(model_name+'.engine', models_path+model_name+'.engine')

        # Export the class names dictionary as a file
        with open(model_labels_path, 'w') as f:
            for i in range(len(class_names)):
                f.write("%d, %s\n" % (i, class_names[i]))
                
    # Load the model
    model = YOLO(models_path+model_name+'.pt')

    # Load class names as a list
    with open(model_labels_path, 'r') as f:
        class_names = f.read().splitlines()
    class_names = [name.split(', ')[1] for name in class_names]

    while not exit_signal:
        if run_signal:
            lock.acquire()

            img = cv2.cvtColor(image_net, cv2.COLOR_BGRA2RGB)
            # https://docs.ultralytics.com/modes/predict/#video-suffixes
            det = model.predict(img, save=False, imgsz=img_size, conf=conf_thres, iou=iou_thres)[0].cpu().numpy().boxes

            # ZED CustomBox format (with inverse letterboxing tf applied)
            detections = detections_to_custom_box(det)
            lock.release()
            run_signal = False
        sleep(0.001)

# Wrap data into ROS ObjectStamped message
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

        pos = obj.position
        obj_msg.position = [pos[0], pos[1], pos[2]]
        pos_cov = obj.position_covariance
        obj_msg.position_covariance = [pos_cov[0], pos_cov[1], pos_cov[2], pos_cov[3], pos_cov[4], pos_cov[5]]
        vel = obj.velocity
        obj_msg.velocity = [vel[0], vel[1], vel[2]]
        obj_msg.tracking_available = True
        if repr(obj.tracking_state) == "OFF":
            obj_msg.tracking_state = 0
        elif repr(obj.tracking_state) == "OK":
            obj_msg.tracking_state = 1
        else:
            obj_msg.tracking_state = 2
        bbox_3d = obj.bounding_box
        if len(bbox_3d) == 8:
            obj_msg.bounding_box_3d.corners[0].kp = [bbox_3d[0][0], bbox_3d[0][1], bbox_3d[0][2]]
            obj_msg.bounding_box_3d.corners[1].kp = [bbox_3d[1][0], bbox_3d[1][1], bbox_3d[1][2]]
            obj_msg.bounding_box_3d.corners[2].kp = [bbox_3d[2][0], bbox_3d[2][1], bbox_3d[2][2]]
            obj_msg.bounding_box_3d.corners[3].kp = [bbox_3d[3][0], bbox_3d[3][1], bbox_3d[3][2]]
            obj_msg.bounding_box_3d.corners[4].kp = [bbox_3d[4][0], bbox_3d[4][1], bbox_3d[4][2]]
            obj_msg.bounding_box_3d.corners[5].kp = [bbox_3d[5][0], bbox_3d[5][1], bbox_3d[5][2]]
            obj_msg.bounding_box_3d.corners[6].kp = [bbox_3d[6][0], bbox_3d[6][1], bbox_3d[6][2]]
            obj_msg.bounding_box_3d.corners[7].kp = [bbox_3d[7][0], bbox_3d[7][1], bbox_3d[7][2]]
        obj_list.append(obj_msg)         
    ros_msg.objects = obj_list
    return ros_msg  

def local_to_base_link_transform(msg, tfBuffer):
    try:
        lct = tfBuffer.get_latest_common_time("base_link", CAMERA_NAME + "_left_camera_frame")
        transform = tfBuffer.lookup_transform("base_link", CAMERA_NAME + "_left_camera_frame", lct, rospy.Duration(0.1))
        for obj in msg.objects:
            p = PointStamped()
            p.point.x = obj.position[0]
            p.point.y = obj.position[1]
            p.point.z = obj.position[2]
            obj.position = tf2_geometry_msgs.do_transform_point(p, transform)
            obj.position = [obj.position.point.x, obj.position.point.y, obj.position.point.z]
            for corner in obj.bounding_box_3d.corners:
                p.point.x = corner.kp[0]
                p.point.y = corner.kp[1]
                p.point.z = corner.kp[2]
                corner.kp = tf2_geometry_msgs.do_transform_point(p, transform)
                corner.kp = [corner.kp.point.x, corner.kp.point.y, corner.kp.point.z]
        msg.header.frame_id = "base_link"
    except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException):
        print("Failed to transform object from local to base_link frame")
    return msg

def get_reachable_objects(objects, gripper_width):
    """
    Return only objects whose bounding box width is ≤ gripper_width × 2
    """
    threshold = gripper_width * 2.2
    reachable = []
    for obj in objects:
        if len(obj.bounding_box_3d.corners) >= 2:
            x0 = obj.bounding_box_3d.corners[0].kp[0]
            x1 = obj.bounding_box_3d.corners[1].kp[0]
            box_width = abs(x1 - x0)
            if box_width <= threshold:
                reachable.append(obj)
    return reachable

def publish_object_target(obj, target_pub):
    x, y, z = obj.position
    rospy.loginfo(f"[DETECTION] x={x:.2f}, y={y:.2f}, z={z:.2f}, id={obj.instance_id}")

    yaw = 0.0  # No rotation tracking yet
    with open("/tmp/robot_pose.txt", "w") as f:
        f.write(f"{-x},{-y},{yaw}")
    rospy.loginfo("[POSE] Saved relative return position.")

    processed_ids.add(obj.instance_id)

    # Try to calculate width from bounding box
    width_cm = 5.5  # Default fallback
    if len(obj.bounding_box_3d.corners) >= 2:
        x0 = obj.bounding_box_3d.corners[0].kp[0]
        x1 = obj.bounding_box_3d.corners[1].kp[0]
        detected_width = abs(x1 - x0) * 100  # meters to cm

        if detected_width <= 12.5:
            width_cm = detected_width * 0.75
            rospy.loginfo(f"[DETECTION] Using detected width: {width_cm:.2f} cm")
        else:
            rospy.logwarn(f"[DETECTION] Detected width {detected_width:.2f} cm too large, using default 5.5 cm")

    # Publish x, y, z, width
    msg = Float32MultiArray()
    msg.data = [x, y, z, width_cm]
    target_pub.publish(msg)

    with open("/tmp/last_z.txt", "w") as f:
        f.write(str(z))

def publish_object_target_pending(obj, target_pub):
    global pending_obj_id

    x, y, z = obj.position
    rospy.loginfo(f"[DETECTION] x={x:.2f}, y={y:.2f}, z={z:.2f}, id={obj.instance_id}")

    yaw = 0.0
    with open("/tmp/robot_pose.txt", "w") as f:
        f.write(f"{-x},{-y},{yaw}")
    rospy.loginfo("[POSE] Saved relative return position.")

    # Only set pending, don't mark as processed
    pending_obj_id = obj.instance_id

    if len(obj.bounding_box_3d.corners) >= 2:
        x0 = obj.bounding_box_3d.corners[0].kp[0]
        x1 = obj.bounding_box_3d.corners[1].kp[0]
        detected_width = abs(x1 - x0) * 100
        if detected_width <= 12.5:
            width_cm = detected_width
            rospy.loginfo(f"[DETECTION] Using detected width: {width_cm:.2f} cm")
        else:
            rospy.logwarn(f"[DETECTION] Detected width {detected_width:.2f} cm too large—using 5.5 cm")
            width_cm = 5.5
    else:
        width_cm = 5.5

    msg = Float32MultiArray()
    msg.data = [x, y, z, width_cm]
    target_pub.publish(msg)

    with open("/tmp/last_z.txt", "w") as f:
        f.write(str(z))

previous_buttons = [0] * 12  # Global

def joy_callback(data):
    global previous_buttons

    # Detect rising edge (pressed now, wasn't before)
    joystick_state["left_pressed"] = data.buttons[0] == 1 and previous_buttons[0] == 0
    joystick_state["right_pressed"] = data.buttons[1] == 1 and previous_buttons[1] == 0

    previous_buttons = data.buttons

def is_object_reachable_by_lidar(obj, scan):
    if scan is None:
        rospy.logwarn("[SCENARIO 4] No LIDAR data yet.")
        return True  # Allow by default if no scan

    x, y = obj.position[0], obj.position[1]
    distance = math.hypot(x, y)
    angle = math.atan2(y, x)

    index = int((angle - scan.angle_min) / scan.angle_increment)

    if 0 <= index < len(scan.ranges):
        lidar_distance = scan.ranges[index]

        # Log for debug
        rospy.loginfo(f"[LIDAR] Object at angle {math.degrees(angle):.1f}° → index {index}, "
                      f"LIDAR={lidar_distance:.2f}m, Object={distance:.2f}m")

        # If object is further than box (i.e., less than LIDAR reading), allow
        if distance < lidar_distance - 0.1:
            return True
        else:
            rospy.logwarn(f"[LIDAR] Object blocked at {distance:.2f}m vs LIDAR {lidar_distance:.2f}m")
            return False
    else:
        rospy.logwarn(f"[LIDAR] Angle index {index} out of range for scan")
        return True

# Holds the object we’ve most recently asked the robot to pick,
# but not yet confirmed by the gripper.
pending_obj_id = None

def main():
    global image_net, exit_signal, run_signal, detections, class_names, selection_state

    GRIPPER_OPEN_MAX_M = 0.125  # Max open width in meters (from gripper script)

    # Define ROS publisher 
    pub_l = rospy.Publisher(CAMERA_NAME+'/od_yolo', zed_msgs.ObjectsStamped, queue_size=50)
    pub_g = rospy.Publisher(CAMERA_NAME+'/od_yolo_base_link_frame', zed_msgs.ObjectsStamped, queue_size=50)

    target_pub = rospy.Publisher("/object_target", Float32MultiArray, queue_size=10)

    tfBuffer = tf2_ros.Buffer()
    listener = tf2_ros.TransformListener(tfBuffer)
    br = tf2_ros.TransformBroadcaster()

    rospy.Subscriber("joy", Joy, joy_callback)
    rospy.Subscriber("/scan", LaserScan, scan_callback)

    capture_thread = Thread(target=torch_thread, kwargs={'model_name': opt.model_name, 'img_size': opt.img_size, "conf_thres": opt.conf_thres})
    capture_thread.start()

    print("Initializing Camera...")

    zed = sl.Camera()

    input_type = sl.InputType()
    if opt.svo is not None:
        input_type.set_from_svo_file(opt.svo)
        
    # Create a InitParameters object and set configuration parameters
    init_params = sl.InitParameters(input_t=input_type, svo_real_time_mode=True)
    init_params.coordinate_units = sl.UNIT.METER
    init_params.depth_mode = sl.DEPTH_MODE.ULTRA  # QUALITY
    init_params.coordinate_system = sl.COORDINATE_SYSTEM.RIGHT_HANDED_Z_UP_X_FWD
    init_params.depth_maximum_distance = 50

    runtime_params = sl.RuntimeParameters()
    status = zed.open(init_params)

    if status != sl.ERROR_CODE.SUCCESS:
        print(repr(status))
        exit()

    image_left_tmp = sl.Mat()

    print("Initialized Camera")

    positional_tracking_parameters = sl.PositionalTrackingParameters()
    # If the camera is static, uncomment the following line to have better performances and boxes sticked to the ground.
    # positional_tracking_parameters.set_as_static = True
    zed.enable_positional_tracking(positional_tracking_parameters)

                    
    obj_param = sl.ObjectDetectionParameters()
    obj_param.detection_model = sl.OBJECT_DETECTION_MODEL.CUSTOM_BOX_OBJECTS
    obj_param.enable_tracking = True
    zed.enable_object_detection(obj_param)

    objects = sl.Objects()
    obj_runtime_param = sl.ObjectDetectionRuntimeParameters()

    try:
        while rospy.is_shutdown() is False:
            if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
                # -- Get the image
                lock.acquire()
                zed.retrieve_image(image_left_tmp, sl.VIEW.LEFT)
                image_net = image_left_tmp.get_data()
                lock.release()
                run_signal = True

                # -- Detection running on the other thread
                while run_signal:
                    sleep(0.001)

                # Wait for detections
                lock.acquire()
                # -- Ingest detections
                zed.ingest_custom_box_objects(detections)
                lock.release()
                zed.retrieve_objects(objects, obj_runtime_param)
            
                # Publish in ROS as an ObjectStamped message
                # ros_msg = ros_wrapper(objects)
                ros_msg = ros_wrapper(objects)
                ros_msg = local_to_base_link_transform(ros_msg, tfBuffer)
                
                if opt.scenario == 1:
                    for obj in ros_msg.objects:
                        if obj.label == "bottle" and obj.instance_id not in processed_ids:
                            publish_object_target(obj, target_pub)
                            break

                elif opt.scenario == 2:
                    # Get all objects that are grabbable based on width
                    filtered_objects = [obj for obj in ros_msg.objects if obj.label not in ["tv", "chair"]]
                    grabbable_objects = get_reachable_objects(filtered_objects, gripper_width=GRIPPER_OPEN_MAX_M)
                    grabbable_objects.sort(key=lambda o: -o.position[1])

                    rospy.loginfo(f"[DEBUG] Found {len(grabbable_objects)} graspable objects:")
                    for i, obj in enumerate(grabbable_objects):
                        if len(obj.bounding_box_3d.corners) >= 2:
                            x0 = obj.bounding_box_3d.corners[0].kp[0]
                            x1 = obj.bounding_box_3d.corners[1].kp[0]
                            width = abs(x1 - x0)
                            rospy.loginfo(f"  [{i}] Label: {obj.label}, Width: {width:.3f} m, Pos: ({obj.position[0]:.2f}, {obj.position[1]:.2f})")

                    if len(grabbable_objects) >= 2:
                        selected_obj = None
                        if joystick_state["left_pressed"]:
                            selected_obj = grabbable_objects[0]
                        elif joystick_state["right_pressed"]:
                            selected_obj = grabbable_objects[1]

                        if selected_obj and selected_obj.instance_id not in processed_ids:
                            publish_object_target(selected_obj, target_pub)

                elif opt.scenario == 3:
                    reachable = get_reachable_objects(
                        [obj for obj in ros_msg.objects if obj.label not in ["tv", "chair"]],
                        gripper_width=GRIPPER_OPEN_MAX_M
                    )
                    if not reachable:
                        continue

                    # Sort left-to-right (Y axis), flip if needed
                    reachable.sort(key=lambda o: -o.position[1])  # or use .position[0] depending on axis

                    # Ensure selected_idx stays in bounds
                    max_index = len(reachable) - 1
                    selected_idx = selection_state["selected_idx"]

                    # Update selection on button press
                    if joystick_state["right_pressed"]:
                        if selected_idx < max_index:
                            selected_idx += 1
                            rospy.loginfo(f"[SELECTION] → Selected object: {reachable[selected_idx].label}")
                        else:
                            # Already at rightmost, just reprint
                            rospy.loginfo(f"[SELECTION] → Still at rightmost object: {reachable[selected_idx].label}")
                        selection_state["last_update_time"] = time.time()
                        selection_state["active"] = True

                    elif joystick_state["left_pressed"]:
                        if selected_idx > 0:
                            selected_idx -= 1
                            rospy.loginfo(f"[SELECTION] ← Selected object: {reachable[selected_idx].label}")
                        else:
                            # Already at leftmost, just reprint
                            rospy.loginfo(f"[SELECTION] ← Still at leftmost object: {reachable[selected_idx].label}")
                        selection_state["last_update_time"] = time.time()
                        selection_state["active"] = True

                    # Update selected index
                    selection_state["selected_idx"] = selected_idx

                    # Check for auto-confirmation timeout
                    if selection_state["active"]:
                        elapsed = time.time() - selection_state["last_update_time"]
                        if elapsed > 5:  # 5 second wait for confirmation
                            selected_obj = reachable[selected_idx]
                            if selected_obj.instance_id not in processed_ids:
                                rospy.loginfo(f"[SELECTION] No input after 5s. Grabbing: {selected_obj.label}")
                                publish_object_target_pending(selected_obj, target_pub)
                                processed_ids.add(selected_obj.instance_id)
                                selection_state["active"] = False  # Reset state

                elif opt.scenario == 4:
                    filtered = [obj for obj in ros_msg.objects if obj.label not in ["tv", "chair"]]
                    grabbable = get_reachable_objects(filtered, GRIPPER_OPEN_MAX_M)
                    rospy.loginfo("[SCENARIO 4] Grabbable objects before LIDAR filtering:")
                    for obj in grabbable:
                        label = obj.label
                        pos = obj.position
                        rospy.loginfo(f" - {label} at (x={pos[0]:.2f}, y={pos[1]:.2f})")

                    valid = [obj for obj in grabbable if is_object_reachable_by_lidar(obj, latest_scan)]

                    rospy.loginfo("[SCENARIO 4] Grabbable objects after LIDAR filtering:")
                    for obj in valid:
                        rospy.loginfo(f" - {obj.label} at (x={obj.position[0]:.2f}, y={obj.position[1]:.2f})")

                    rospy.loginfo(f"[SCENARIO 4] Found {len(valid)} objects not blocked by LIDAR")
                    
                    for obj in valid:
                        if obj.instance_id not in processed_ids:
                            publish_object_target(obj, target_pub)
                            break

                # Reset joystick states after processing
                joystick_state["left_pressed"] = False
                joystick_state["right_pressed"] = False
                

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
    rospy.init_node("zed_yolo_ros", anonymous=False)
    rospy.loginfo("ZED YOLO node started")
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_name', type=str, default='yolo11l', help='model path(s)')
    parser.add_argument('--svo', type=str, default=None, help='optional svo file')
    parser.add_argument('--img_size', type=int, default=416, help='inference size (pixels)')
    parser.add_argument('--conf_thres', type=float, default=0.4, help='object confidence threshold')
    parser.add_argument('--scenario', type=int, default=1, help='Scenario number to run (1-5)')
    opt = parser.parse_args()

    with torch.no_grad():
        main()
