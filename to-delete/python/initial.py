#!/home/pi/Desktop/tf/bin python
import time
import serial
import requests
import json
import RPi.GPIO as GPIO


GPIO.setmode(GPIO.BOARD)
GPIO.setup(11, GPIO.IN)


raw_data = ''
data = ''


ser = serial.Serial(
    port='/dev/ttyS0',
    baudrate = 9600,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    bytesize = serial.EIGHTBITS,
    timeout = 0 # 0 so that it continues to check what's inside of the serial port with no delay
)


def read(ch):
    global raw_data
    global data
    if(ch == '}'): # This is the end of message
        raw_data += ch
        
        if(raw_data[0] == '{' and raw_data[len(raw_data)-1] == '}'):
            data = json.loads(raw_data)
            print(data)
            
            # print(jsonLine.get("command"))
            # print(jsonLine.get("message"))
    
        raw_data = '' # Resets message
    else:
        raw_data += ch
        
def send(command, message):
    data = {}
    data["COMMAND"] = command
    data["MESSAGE"] = message
    data["OWNER"] = "Jesse" # Change this
    print(data)
    data = json.dumps(data)
    
    if ser.isOpen():
        ser.write(data.encode('ascii'))
        ser.flush()


print("Initializing drone...") # Starting line

f = open("../triggers/isConnected.txt","w+")
f.write("False\n")
f.close()

f = open("../triggers/isDeploying.txt","w+")
f.write("False\n")
f.close()

while True:
    f = open("../triggers/isConnected.txt", "r")
    if f.mode == "r":
        isConnected = f.readline().strip("\n")
        if isConnected == "True":
            exit()
        
    if GPIO.input(11) == GPIO.HIGH:
        print("Trying to connect to base station...")
        send("CONNECT", "Connecting")
        