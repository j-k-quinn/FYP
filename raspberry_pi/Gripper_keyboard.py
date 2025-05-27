import RPi.GPIO as GPIO
import time

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

print("\nGripper Control via SSH")
print("Commands:")
print("w = Lift Up")
print("s = Lift Down")
print("a = Gripper Open")
print("d = Gripper Close")
print("q = Stop Lift")
print("e = Stop Gripper")
print("x = Exit\n")

try:
    while True:
        key = input("Enter command: ").lower().strip()

        if key == 'w':
            move_up()
        elif key == 's':
            move_down()
        elif key == 'a':
            open_gripper()
        elif key == 'd':
            close_gripper()
        elif key == 'q':
            stop_lift()
        elif key == 'e':
            stop_gripper()
        elif key == 'x':
            print("Exiting...")
            break
        else:
            print("Invalid command.")
        
        # Wait briefly to allow movement before stopping
        time.sleep(0.5)
        stop_lift()
        stop_gripper()

except KeyboardInterrupt:
    pass

finally:
    stop_lift()
    stop_gripper()
    GPIO.cleanup()
