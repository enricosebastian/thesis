import serial
import time

ser = serial.Serial("/dev/ttyS0", 9600)

while True:
    ser.write("Hello from Rpi\n".encode('utf-8'))
    time.sleep(1)