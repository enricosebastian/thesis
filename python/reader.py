#!/home/pi/Desktop/tf/bin python
import time
import serial
import requests
import json

import RPi.GPIO as GPIO


GPIO.setmode(GPIO.BOARD)
GPIO.setup(13, GPIO.IN)


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


def send(command, message):
    data = {}
    data["COMMAND"] = command
    data["MESSAGE"] = message
    data["OWNER"] = "Jesse" # Change this
    
    print("Sending: ")
    print(data)
    print("\n")
    
    data = json.dumps(data)
    
    if ser.isOpen():
        ser.write(data.encode('ascii'))
        ser.flush()
        

def read(ch):
    global raw_data
    global data
    
    if(ch == '}'): # This is the end of message
        raw_data += ch
        
        if(raw_data[0] == '{' and raw_data[len(raw_data)-1] == '}'):
            data = json.loads(raw_data)
            
            print("Receiving: ")
            print(data)
            print("\n")
            
            interpet_command(data)
    
        raw_data = '' # Resets message
    else:
        raw_data += ch

def writeFile(fileName, value):
    f = open("../triggers/"+fileName+".txt","w+")
    f.write(value)
    f.close()
    
def readFile(fileName):
    f = open("../triggers/"+fileName+".txt", "r")
    if f.mode == "r":
        value = f.readline().strip("\n")
        return value
    
    return "False"

def interpet_command(data):
    if(data.get("COMMAND") == "CONNECT-REPLY"):
        if(data.get("MESSAGE") == "True"):
            writeFile("isConnected", "True")
            
    elif(data.get("COMMAND") == "REPLY"):
        if(data.get("MESSAGE") == "False"):
            print("Whoa, something went wrong...")
            
    elif(data.get("COMMAND") == "DEPLOY"):
        if(data.get("MESSAGE") == "Jesse"):
            writeFile("isDeploying", "True")
            send("DEPLOY-REPLY", "True")
            # detect.main()


# Starting line
print("Command reader...") 
writeFile("isDeploying", "False") # Make sure they're all false first...
writeFile("isConnected", "False")

while True:
    ch = ser.read(1).decode('utf-8') #Continously reads the input
    
    if ch != '':
        read(ch)
        
    if GPIO.input(13) == GPIO.HIGH:
        print("Restart")
        