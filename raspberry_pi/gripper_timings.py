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
    GPIO.output(IN1, GPIO.HIGH)
    GPIO.output(IN2, GPIO.LOW)
    pwm_lift.ChangeDutyCycle(80)

def move_down():
    GPIO.output(IN1, GPIO.LOW)
    GPIO.output(IN2, GPIO.HIGH)
    pwm_lift.ChangeDutyCycle(80)

def stop_lift():
    GPIO.output(IN1, GPIO.LOW)
    GPIO.output(IN2, GPIO.LOW)
    pwm_lift.ChangeDutyCycle(0)

def open_gripper():
    GPIO.output(IN3, GPIO.HIGH)
    GPIO.output(IN4, GPIO.LOW)
    pwm_grip.ChangeDutyCycle(80)

def close_gripper():
    GPIO.output(IN3, GPIO.LOW)
    GPIO.output(IN4, GPIO.HIGH)
    pwm_grip.ChangeDutyCycle(80)

def stop_gripper():
    GPIO.output(IN3, GPIO.LOW)
    GPIO.output(IN4, GPIO.LOW)
    pwm_grip.ChangeDutyCycle(0)

# Timing Functions
def time_movement(start_func, stop_func, movement_name, duration=10):
    """
    Times how long a movement takes by running it for a specified duration
    and measuring the actual time elapsed.
    """
    print(f"\nTiming {movement_name}...")
    print("Press Ctrl+C when the movement is complete (or let it run for max duration)")
    
    start_time = time.time()
    start_func()
    
    try:
        time.sleep(duration)  # Maximum duration to prevent damage
        stop_func()
        end_time = time.time()
        elapsed = end_time - start_time
        print(f"Movement completed in maximum time: {elapsed:.2f} seconds")
        return elapsed
    except KeyboardInterrupt:
        stop_func()
        end_time = time.time()
        elapsed = end_time - start_time
        print(f"Movement stopped manually after: {elapsed:.2f} seconds")
        return elapsed

def time_full_range_movement(start_func, stop_func, movement_name, estimated_duration):
    """
    Times a full range movement (like top to bottom or open to close)
    """
    print(f"\nTiming full range {movement_name}...")
    print("Make sure the mechanism is at the starting position!")
    input("Press Enter when ready to start timing...")
    
    start_time = time.time()
    start_func()
    
    try:
        # Run for estimated duration, but allow manual stop
        time.sleep(estimated_duration)
        stop_func()
        end_time = time.time()
        elapsed = end_time - start_time
        print(f"Full range {movement_name} took: {elapsed:.2f} seconds")
        return elapsed
    except KeyboardInterrupt:
        stop_func()
        end_time = time.time()
        elapsed = end_time - start_time
        print(f"Movement stopped manually after: {elapsed:.2f} seconds")
        return elapsed

def manual_timing_test():
    """
    Interactive timing test where you manually control when to stop
    """
    print("\n=== MANUAL TIMING TEST ===")
    print("You'll control when each movement starts and stops")
    
    movements = [
        ("Top to Bottom", move_down, stop_lift),
        ("Bottom to Top", move_up, stop_lift),
        ("Open to Close", close_gripper, stop_gripper),
        ("Close to Open", open_gripper, stop_gripper)
    ]
    
    results = {}
    
    for name, start_func, stop_func in movements:
        print(f"\n--- {name} ---")
        print("Position the mechanism at the starting point")
        input("Press Enter when ready to start...")
        
        print(f"Starting {name}... Press Ctrl+C when complete!")
        start_time = time.time()
        start_func()
        
        try:
            while True:
                time.sleep(0.1)
        except KeyboardInterrupt:
            stop_func()
            end_time = time.time()
            elapsed = end_time - start_time
            results[name] = elapsed
            print(f"{name} completed in: {elapsed:.2f} seconds")
            time.sleep(1)  # Brief pause between tests
    
    return results

def automated_timing_test():
    """
    Automated timing test with estimated durations
    WARNING: Make sure your mechanism won't be damaged by running for these durations
    """
    print("\n=== AUTOMATED TIMING TEST ===")
    print("WARNING: This will run motors for estimated durations.")
    print("Make sure your mechanism won't be damaged!")
    
    if input("Continue? (y/n): ").lower() != 'y':
        return {}
    
    # Adjust these durations based on your mechanism
    tests = [
        ("Top to Bottom", move_down, stop_lift, 5.0),
        ("Bottom to Top", move_up, stop_lift, 5.0),
        ("Open to Close", close_gripper, stop_gripper, 3.0),
        ("Close to Open", open_gripper, stop_gripper, 3.0)
    ]
    
    results = {}
    
    for name, start_func, stop_func, duration in tests:
        print(f"\nPosition mechanism for {name}")
        input("Press Enter when ready...")
        elapsed = time_full_range_movement(start_func, stop_func, name, duration)
        results[name] = elapsed
        time.sleep(2)  # Pause between tests
    
    return results

# Main timing program
def main():
    try:
        print("=== GRIPPER TIMING TOOL ===")
        print("Choose timing method:")
        print("1. Manual control (you stop each movement)")
        print("2. Automated with estimated durations")
        print("3. Individual movement timing")
        
        choice = input("Enter choice (1-3): ")
        
        if choice == '1':
            results = manual_timing_test()
        elif choice == '2':
            results = automated_timing_test()
        elif choice == '3':
            # Individual timing examples
            print("\nExample: Timing upward movement...")
            time_movement(move_up, stop_lift, "upward movement", 5)
        else:
            print("Invalid choice")
            return
        
        # Display results
        if choice in ['1', '2'] and results:
            print("\n=== TIMING RESULTS ===")
            for movement, duration in results.items():
                print(f"{movement}: {duration:.2f} seconds")
                
            # Save results to file
            with open('gripper_timings.txt', 'w') as f:
                f.write("Gripper Timing Results\n")
                f.write("=" * 25 + "\n")
                for movement, duration in results.items():
                    f.write(f"{movement}: {duration:.2f} seconds\n")
            print("\nResults saved to 'gripper_timings.txt'")
            
    except KeyboardInterrupt:
        print("\nTiming interrupted by user")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        # Cleanup
        stop_lift()
        stop_gripper()
        GPIO.cleanup()
        print("GPIO cleaned up")

if __name__ == "__main__":
    main()
