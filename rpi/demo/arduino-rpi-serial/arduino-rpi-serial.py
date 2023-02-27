import serial
import time
import os
import RPi.GPIO as GPIO

# ser = serial.Serial("/dev/ttyACM0", 9600)

ser = serial.Serial("/dev/ttyS0", 9600)

detectionPin = 7
offPin = 11

GPIO.setmode(GPIO.BOARD)
GPIO.setup(detectionPin, GPIO.IN)
GPIO.setup(offPin, GPIO.IN)

while True:
	message = "DETE ALL ALL LEFT\n"
	print("Sending", message)
	ser.write(message.encode('utf-8'))
	time.sleep(1)

	if GPIO.input(offPin) == True:
		os.system("sudo shutdown -h now")