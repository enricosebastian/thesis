import serial
import time

ser = serial.Serial("/dev/ttyS0", 9600)

while True:
    message = "DETE ALL ALL DONE\n"
    print("Sending", message)
    ser.write(message.encode('utf-8'))
    time.sleep(1)