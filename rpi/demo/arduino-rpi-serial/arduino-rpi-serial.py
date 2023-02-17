import serial
import time

ser = serial.Serial("/dev/ttyACM0", 9600)

while True:
    message = "DETE ALL ALL LEFT\n"
    print("Sending", message)
    ser.write(message.encode('utf-8'))
    time.sleep(1)