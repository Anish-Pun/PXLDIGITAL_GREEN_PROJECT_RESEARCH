import serial
import time
import RPi.GPIO as GPIO

# ---------------- Configuration ----------------
SERIAL_PORT = '/dev/ttyAMA0'
BAUDRATE = 9600
TIMEOUT = 1

AUX_PIN = 18
M0_PIN = 21
M1_PIN = 20

# ---------------- Setup GPIO ----------------
GPIO.setmode(GPIO.BCM)
GPIO.setup(AUX_PIN, GPIO.IN)
GPIO.setup(M0_PIN, GPIO.OUT)
GPIO.setup(M1_PIN, GPIO.OUT)

def wait_aux_ready():
    """Wait until AUX pin goes HIGH (LoRa ready)."""
    while GPIO.input(AUX_PIN) == 0:
        time.sleep(0.01)

def enter_at_mode():
    """Enter AT command mode (M0=1, M1=1)."""
    GPIO.output(M0_PIN, GPIO.HIGH)
    GPIO.output(M1_PIN, GPIO.HIGH)
    time.sleep(0.1)
    wait_aux_ready()
    print("Entered AT command mode")

def exit_at_mode():
    """Return to normal transmission mode (M0=0, M1=0)."""
    GPIO.output(M0_PIN, GPIO.LOW)
    GPIO.output(M1_PIN, GPIO.LOW)
    time.sleep(0.1)
    wait_aux_ready()
    print("Exited AT command mode")

def open_serial():
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
    print(f"Connected to {SERIAL_PORT} at {BAUDRATE}bps")
    wait_aux_ready()
    return ser

def send_at_command(ser, command):
    """Send AT command and return response."""
    full_cmd = (command + '\r\n').encode('utf-8')
    wait_aux_ready()
    ser.write(full_cmd)
    wait_aux_ready()
    time.sleep(0.1)
    response = ser.read_all().decode(errors='ignore').strip()
    return response

def receive_data(ser):
    """Continuously read data from LoRa module."""
    print("Listening for incoming LoRa data...")
    try:
        while True:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                try:
                    text = data.decode('utf-8', errors='replace')
                except:
                    text = repr(data)
                print("AUX:", GPIO.input(AUX_PIN))
                print("Received:", text)
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Stopping reception...")
        
def factory_reset(ser):
    """Reset E220 module to factory defaults."""
    print("\nSending factory reset command...")
    response = send_at_command(ser, "AT+DEFAULT")
    print("Factory reset response:", response)
    time.sleep(0.5)  # wait a bit for module to apply defaults
    wait_aux_ready()
        
def main():
    ser = open_serial()
    enter_at_mode()
    factory_reset(ser)
    # Example AT queries
    commands = [
        "AT+DEVTYPE=?",
        "AT+FWCODE=?",
        "AT+UART=?",
        "AT+RATE=?",
        "AT+PACKET=?",
        "AT+ADDR=?",
        "AT+CHANNEL=?",
        "AT+MODE=?",
        "AT+TRANS=?",
    ]

    for cmd in commands:
        print(f"\nSending: {cmd}")
        response = send_at_command(ser, cmd)
        print(f"Response: {response}")
        time.sleep(0.5)
    exit_at_mode()
    receive_data(ser)
    ser.close()

if __name__ == "__main__":
    main()
