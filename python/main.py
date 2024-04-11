import serial
import uinput

ser = serial.Serial('/dev/ttyACM0', 9600) # Change to rfcomm0 if using bluetooth

# Create new gamepad device (More codes here https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h?h=v4.7)
device = uinput.Device([ 
    uinput.BTN_A,
    uinput.BTN_B,
    uinput.BTN_X,
    uinput.BTN_Y,
    uinput.BTN_TL,
    uinput.BTN_TR,
    uinput.BTN_TL2, 
    uinput.BTN_TR2,
    uinput.BTN_SELECT,
    uinput.BTN_START,
    uinput.BTN_THUMBL,
    uinput.BTN_THUMBR,
    uinput.BTN_DPAD_UP,
    uinput.BTN_DPAD_DOWN,
    # uinput.BTN_DPAD_LEFT,
    # uinput.BTN_DPAD_RIGHT,
    uinput.ABS_X + (0, 255, 0, 0),  # (min, max, fuzz, flat)
    uinput.ABS_Y + (0, 255, 0, 0),
])

button_quantity = 14 # Quantity of buttons in your controller (without joystick)

def parse_data(data):
    button = data[0]  # 0 for A, 1 for B, 2 for X, 3 for Y
    value = int.from_bytes(data[1:3], byteorder='little', signed=True)
    print(f"Received data: {data}")
    print(f"button: {button}, value: {value}")
    return button, value

def emulate_controller(button, value):
    buttons = [uinput.BTN_A, uinput.BTN_B, uinput.BTN_X, uinput.BTN_Y, uinput.BTN_TL, uinput.BTN_TR, uinput.BTN_TL2, uinput.BTN_TR2, uinput.BTN_SELECT, uinput.BTN_START, uinput.BTN_THUMBL, uinput.BTN_THUMBR]
    axes = [uinput.ABS_X, uinput.ABS_Y]

    if button < button_quantity:  # Amoung of buttons in your controller
        device.emit(buttons[button], value)
    else:  # Joystick movement
        device.emit(axes[button - button_quantity], value)

try:
    # sync package
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(1)
            if data == b'\xff':
                break

        # Read 4 bytes from UART
        data = ser.read(3)
        button, value = parse_data(data)
        emulate_controller(button, value)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()