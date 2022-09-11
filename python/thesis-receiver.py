#!/home/pi/Desktop/tflite/tf/bin python
import time
import serial
import requests
import json

line = ''

ser = serial.Serial(
    port='/dev/ttyS0',
    baudrate = 9600,
    parity = serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0 # 0 so that it continues to check what's inside of the serial port
)

def readCommand(c):
    global line
    
    if(c == '}'):
        line += c
        
        if(line[0] == '{' and line[len(line)-1] == '}'):
            jsonLine = json.loads(line)
            print(jsonLine)
            # print(jsonLine.get("command"))
            # print(jsonLine.get("message"))
    
        line = '' # Resets message
    else:
        line += c


while True:
    c = ser.read(1).decode('utf-8')
    if c != '':
        readCommand(c)