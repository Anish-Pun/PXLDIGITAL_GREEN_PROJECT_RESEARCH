
import serial
import time

# Change these to match your setup:
SERIAL_PORT = '/dev/ttyAMA0'     # or '/dev/ttyS0', check your Pi version & configuration
BAUDRATE    = 9600               # must match the sender’s UART baud (in your ESP32 code it’s Serial.begin(9600))
TIMEOUT     = 1.0                # seconds
def main():
    try:
        ser = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, timeout=TIMEOUT)
        print(f"Listening on {SERIAL_PORT}, {BAUDRATE}bps")
    except serial.SerialException as e:
        print("Could not open serial port:", e)
        return

    while True:
        if ser.in_waiting:
            data = ser.read(ser.in_waiting)  # read all available
            try:
                text = data.decode('utf-8', errors='replace')
            except:
                text = repr(data)
            print("Received:", text)
        time.sleep(0.1)

if __name__ == '__main__':
    main()
