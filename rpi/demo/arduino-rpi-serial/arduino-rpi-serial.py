import serial
import time

ser = serial.Serial("/dev/ttyS0", 9600)

while True:
    message = "DETE ALL ALL LEFT\n"
    Serial.println("Sending", message)
    ser.write(message.encode('utf-8'))
    time.sleep(1)