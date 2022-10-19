import serial
import time
import json

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
    message = input()
    if(message == "true"):
        data = {}
        data["is_found"] = True
        data["origin_x"] = 300
        data = json.dumps(data)
        print(data)
        ser.write(data.encode('utf-8'))
    else:
        data = {}  
        data["is_found"] = False
        data = json.dumps(data)
        print(data)
        ser.write(data.encode('utf-8'))
        

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