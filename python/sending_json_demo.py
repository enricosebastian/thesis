import serial
import time
import json
import os

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)
GPIO.setup(13, GPIO.IN)

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)
ser.flush()

for i in range(10):
    data = {}
    data["is_found"] = True
    data["origin_x"] = 300
    data = json.dumps(data)
    print(data)
    ser.write(data.encode('utf-8'))
    time.sleep(1)
    
while True:
    print(GPIO.input(13))
    
    if GPIO.input(13) == GPIO.HIGH:
        data = {}
        data["is_found"] = True
        data["origin_x"] = 300
        data = json.dumps(data)
        print(data)
        ser.write(data.encode('utf-8'))
        os.system("shutdown -h now")
        time.sleep(1)
        

# for i in range(5):
#     data = {}
#     data["is_found"] = True
#     data["origin_x"] = 300
#     data = json.dumps(data)
#     print(data)
#     ser.write(data.encode('utf-8'))
#     time.sleep(1)
  
# data = {}  
# data["is_found"] = False
# data = json.dumps(data)
# print(data)
# ser.write(data.encode('utf-8'))