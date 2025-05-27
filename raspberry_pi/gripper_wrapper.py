import RPi.GPIO as GPIO
import time

class GripperWrapper:
    def __init__(self):
        # Define GPIO Pins
        self.IN1, self.IN2, self.ENA = 17, 27, 18  # Lift
        self.IN3, self.IN4, self.ENB = 22, 23, 19  # Gripper

        # Setup GPIO
        GPIO.setmode(GPIO.BCM)
        GPIO.setup([self.IN1, self.IN2, self.IN3, self.IN4, self.ENA, self.ENB], GPIO.OUT)

        # Setup PWM
        self.pwm_lift = GPIO.PWM(self.ENA, 1000)
        self.pwm_grip = GPIO.PWM(self.ENB, 1000)
        self.pwm_lift.start(0)
        self.pwm_grip.start(0)

        # Constants
        self.height_top = 77  # cm
        self.height_bottom = 25  # cm
        self.open_max = 12.5  # cm max gripper open distance
        self.current_height = self.height_top
        self.current_open = self.open_max  # current gripper open width in cm

        # Speed in cm per second
        self.speed_up = 52 / 19   # lift speed up
        self.speed_down = 52 / 12 # lift speed down
        self.speed_open = 12.5 / 5     # gripper opening speed
        self.speed_close = 12.5 / 4.5  # gripper closing speed

    def cm_to_time(self, delta_cm, direction):
        if direction == "down":
            return delta_cm / self.speed_down
        elif direction == "up":
            return delta_cm / self.speed_up
        else:
            raise ValueError("Direction must be 'up' or 'down'")

    def move_to_height(self, target_height):
        delta = abs(target_height - self.current_height)
        if delta < 0.5:
            return 0
        direction = "down" if target_height < self.current_height else "up"
        duration = self.cm_to_time(delta, direction)

        if direction == "up":
            self._move_up(duration)
        else:
            self._move_down(duration)
        self.current_height = target_height
        return duration

    def open_gripper(self):
        delta = self.open_max - self.current_open
        if delta < 0.1:
            return 0
        duration = delta / self.speed_open
        self._open_gripper(duration)
        self.current_open = self.open_max
        return duration

    def close_gripper(self):
        delta = self.current_open
        if delta < 0.1:
            return 0
        duration = delta / self.speed_close
        self._close_gripper(duration)
        self.current_open = 0
        return duration

    def set_gripper_width(self, target_width):
        """
        Move gripper to a target width (in cm).
        This opens or closes the gripper relative to current width.
        """
        target_width = max(0, min(target_width, self.open_max))  # Clamp width
        delta = target_width - self.current_open

        if abs(delta) < 0.1:
            return 0  # Already at desired width

        if delta > 0:
            # Need to open more
            duration = delta / self.speed_open
            self._open_gripper(duration)
        else:
            # Need to close more
            duration = (-delta) / self.speed_close
            self._close_gripper(duration)

        self.current_open = target_width
        return duration

    def restart_position(self):
        """Move to top and open gripper."""
        self.move_to_height(self.height_top)
        self.open_gripper()
      
   def pick_object_at_z(self, z, width=None):
        """
        Pick an object at the given Z (meters).
        If width (cm) is provided, close gripper to that width instead of fully closing.
        """
        target_cm = max(self.height_bottom, min(z * 100, self.height_top))
        self.open_gripper()
        self.move_to_height(target_cm)
        if width is not None:
            self.set_gripper_width(width)
        else:
            self.close_gripper()
        self.move_to_height(self.height_top)

    # Hardware Control (directly using your original motor logic)

    def _move_up(self, duration):
        GPIO.output(self.IN1, GPIO.LOW)
        GPIO.output(self.IN2, GPIO.HIGH)
        self.pwm_lift.ChangeDutyCycle(80)
        time.sleep(duration)
        self._stop_lift()

    def _move_down(self, duration):
        GPIO.output(self.IN1, GPIO.HIGH)
        GPIO.output(self.IN2, GPIO.LOW)
        self.pwm_lift.ChangeDutyCycle(80)
        time.sleep(duration)
        self._stop_lift()

    def _stop_lift(self):
        GPIO.output(self.IN1, GPIO.LOW)
        GPIO.output(self.IN2, GPIO.LOW)
        self.pwm_lift.ChangeDutyCycle(0)

    def _open_gripper(self, duration):
        GPIO.output(self.IN3, GPIO.LOW)
        GPIO.output(self.IN4, GPIO.HIGH)
        self.pwm_grip.ChangeDutyCycle(80)
        time.sleep(duration)
        self._stop_gripper()

    def _close_gripper(self, duration):
        GPIO.output(self.IN3, GPIO.HIGH)
        GPIO.output(self.IN4, GPIO.LOW)
        self.pwm_grip.ChangeDutyCycle(80)
        time.sleep(duration)
        self._stop_gripper()

    def _stop_gripper(self):
        GPIO.output(self.IN3, GPIO.LOW)
        GPIO.output(self.IN4, GPIO.LOW)
        self.pwm_grip.ChangeDutyCycle(0)

if __name__ == "__main__":
    try:
        gripper = GripperWrapper()
        gripper._move_up(1)
        gripper.restart_position()             # Start from top/open
        # Example pick with width 8 cm:
        # gripper.pick_object_at_z(0.45, width=8)
    finally:
        GPIO.cleanup()
