#!/home/pi/Desktop/tflite/tf/bin python
import time
import serial
import requests
import json

print("Transmitter terminal") # Just to indicate that this is the receiver script

line = ''

ser = serial.Serial(
    port='/dev/ttyS0',
    baudrate = 9600,
    parity = serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0 # 0 so that it continues to check what's inside of the serial port
)

def sendCommand(command):
    data = {}
    data["COMMAND"] = command
    data["MESSAGE"] = "this is a message"
    data["OWNER"] = "Jesse"
    data = json.dumps(data)
    
    if ser.isOpen():
        ser.write(data.encode('ascii'))
        ser.flush()
    

while True:
    c = ser.read(1).decode('utf-8')
    if c == '':
        sendCommand(input("Input message: "))
    else:
        print("busy...")