import RPi.GPIO as GPIO
import time
import keyboard  # Requires: pip3 install keyboard

# GPIO pin setup
IN1, IN2, ENA = 17, 27, 18  # Lift
IN3, IN4, ENB = 22, 23, 19  # Gripper

GPIO.setmode(GPIO.BCM)
GPIO.setup([IN1, IN2, IN3, IN4, ENA, ENB], GPIO.OUT)

pwm_lift = GPIO.PWM(ENA, 1000)
pwm_grip = GPIO.PWM(ENB, 1000)
pwm_lift.start(0)
pwm_grip.start(0)

def move_down():
    GPIO.output(IN1, GPIO.HIGH)
    GPIO.output(IN2, GPIO.LOW)
    pwm_lift.ChangeDutyCycle(80)

def move_up():
    GPIO.output(IN1, GPIO.LOW)
    GPIO.output(IN2, GPIO.HIGH)
    pwm_lift.ChangeDutyCycle(80)

def stop_lift():
    GPIO.output(IN1, GPIO.LOW)
    GPIO.output(IN2, GPIO.LOW)
    pwm_lift.ChangeDutyCycle(0)

def close_gripper():
    GPIO.output(IN3, GPIO.HIGH)
    GPIO.output(IN4, GPIO.LOW)
    pwm_grip.ChangeDutyCycle(80)

def open_gripper():
    GPIO.output(IN3, GPIO.LOW)
    GPIO.output(IN4, GPIO.HIGH)
    pwm_grip.ChangeDutyCycle(80)

def stop_gripper():
    GPIO.output(IN3, GPIO.LOW)
    GPIO.output(IN4, GPIO.LOW)
    pwm_grip.ChangeDutyCycle(0)

print("Controls:")
print("W/S = Lift Up/Down")
print("A/D = Gripper Open/Close")
print("Q = Stop Lift, E = Stop Gripper")
print("X = Exit")

try:
    while True:
        if keyboard.is_pressed('w'):
            move_up()
        elif keyboard.is_pressed('s'):
            move_down()
        elif keyboard.is_pressed('a'):
            open_gripper()
        elif keyboard.is_pressed('d'):
            close_gripper()
        elif keyboard.is_pressed('q'):
            stop_lift()
        elif keyboard.is_pressed('e'):
            stop_gripper()
        elif keyboard.is_pressed('x'):
            print("Exiting...")
            break
        else:
            # Optional: stop motors if no key is pressed
            stop_lift()
            stop_gripper()

        time.sleep(0.1)

except KeyboardInterrupt:
    pass

finally:
    stop_lift()
    stop_gripper()
    GPIO.cleanup()
