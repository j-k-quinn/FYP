import RPi.GPIO as GPIO
import time

# Define GPIO Pins
IN1, IN2, ENA = 17, 27, 18  # Lift motor (Up/Down)
IN3, IN4, ENB = 22, 23, 19  # Gripper motor (Open/Close)

# GPIO Setup
GPIO.setmode(GPIO.BCM)
GPIO.setup([IN1, IN2, IN3, IN4, ENA, ENB], GPIO.OUT)

# PWM Setup
pwm_lift = GPIO.PWM(ENA, 1000)
pwm_grip = GPIO.PWM(ENB, 1000)
pwm_lift.start(0)
pwm_grip.start(0)

# Motor Control Functions
def move_up():
    if not GPIO.input(LIMIT_UP):  # Stop if upper limit reached
        stop_lift()
        return
    GPIO.output(IN1, GPIO.HIGH)
    GPIO.output(IN2, GPIO.LOW)
    pwm_lift.ChangeDutyCycle(80)

def move_down():
    if not GPIO.input(LIMIT_DOWN):  # Stop if lower limit reached
        stop_lift()
        return
    GPIO.output(IN1, GPIO.LOW)
    GPIO.output(IN2, GPIO.HIGH)
    pwm_lift.ChangeDutyCycle(80)

def stop_lift():
    GPIO.output(IN1, GPIO.LOW)
    GPIO.output(IN2, GPIO.LOW)
    pwm_lift.ChangeDutyCycle(0)

def open_gripper():
    if not GPIO.input(LIMIT_OPEN):  # Stop if fully open
        stop_gripper()
        return
    GPIO.output(IN3, GPIO.HIGH)
    GPIO.output(IN4, GPIO.LOW)
    pwm_grip.ChangeDutyCycle(80)

def close_gripper():
    if not GPIO.input(LIMIT_CLOSE):  # Stop if fully closed
        stop_gripper()
        return
    GPIO.output(IN3, GPIO.LOW)
    GPIO.output(IN4, GPIO.HIGH)
    pwm_grip.ChangeDutyCycle(80)

def stop_gripper():
    GPIO.output(IN3, GPIO.LOW)
    GPIO.output(IN4, GPIO.LOW)
    pwm_grip.ChangeDutyCycle(0)

# Test
try:
    print("Moving up for 2 seconds...")
    move_up()
    time.sleep(2)
    stop_lift()

    print("Opening gripper for 2 seconds...")
    open_gripper()
    time.sleep(2)
    stop_gripper()

except KeyboardInterrupt:
    print("Stopping...")
finally:
    GPIO.cleanup()
