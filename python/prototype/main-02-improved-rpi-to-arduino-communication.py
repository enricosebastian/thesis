import serial
import time
import json
import os

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
GPIO.setup(11, GPIO.IN)

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)
ser.flush()

myName = "DRO1"

isDeployed = False


while True:
    if GPIO.input(11) == GPIO.HIGH:
        isDeployed = True
    
    if GPIO.input(11) == GPIO.LOW:
        isDeployed = False
        
    if isDeployed:
        print("is detecting...")
        
    elif not isDeployed:
        print("is not detecting")
        
        # data = {}
        # print("Hallo ")
        # details = input()
        # data["command"] = "DETE"
        # data["fromName"] = myName
        # data["toName"] = myName
        # data["details"] = details
        
        # data = json.dumps(data)
        # print(data)
        # ser.write(data.encode('utf-8'))