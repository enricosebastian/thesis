import serial
import time
import json
import os

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
GPIO.setup(13, GPIO.IN)

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)
ser.flush()

myName = "DRO1"

while True:
    if GPIO.input(11) == GPIO.HIGH:
        data = {}
        print("Input your details: ")
        details = input()
        data["command"] = "DETE"
        data["fromName"] = myName
        data["toName"] = myName
        data["details"] = details
        
        data = json.dumps(data)
        print(data)
        ser.write(data.encode('utf-8'))