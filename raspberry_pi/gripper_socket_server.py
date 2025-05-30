import socket
from gripper_wrapper import GripperWrapper
import RPi.GPIO as GPIO

HOST = "0.0.0.0"
PORT = 5005

# Adjust these if needed
MAX_GRIPPER_OPEN = 12.5  # cm (max open distance)
GRIPPER_PADDING = 0.5    # cm extra space to avoid crushing

def start_server():
    gripper = GripperWrapper()
    gripper._open_gripper(5.5)
    gripper.restart_position()  # Start fully open at top

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"[PI] Listening on port {PORT}...")

        while True:
            conn, addr = s.accept()
            with conn:
                print(f"[PI] Connected from {addr}")
                data = conn.recv(1024)
                if not data:
                    continue
                try:
                    parts = list(map(float, data.decode().strip().split(",")))

                    if len(parts) == 4:
                        x, y, z, obj_width = parts
                    elif len(parts) == 3:
                        x, y, z = parts
                        obj_width = 5.5  # Default width if not provided
                    else:
                        print(f"[PI] Invalid data format: {parts}")
                        continue

                    print(f"[PI] Received coords: x={x}, y={y}, z={z}, obj_width={obj_width}")

                    if 0.25 <= z <= 0.77:
                        # Calculate closing width (object width plus padding), clamp it so not too wide or narrow
                        close_width = max(1.0, min(obj_width - GRIPPER_PADDING, MAX_GRIPPER_OPEN))
                        print(f"[PI] Closing gripper to: {close_width} cm")
                        
                        # Move to z position first (assuming pick_object_at_z moves to z)
                        gripper.pick_object_at_z(z, close_width)

                        conn.sendall(b"grip_done\n")
                        print("[PI] Sent confirmation: grip_done")

                    else:
                        print("[PI] Z out of range. Ignoring.")

                except Exception as e:
                    print(f"[PI] Error processing message: {e}")

if __name__ == "__main__":
    try:
        start_server()
    finally:
        GPIO.cleanup()
