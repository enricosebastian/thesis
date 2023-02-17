import RPi.GPIO as GPIO

offPin = 11

GPIO.setmode(GPIO.BOARD)
GPIO.setup(offPin, GPIO.IN)

while True:
    if GPIO.input(offPin) == True:
        print("on")
    else:
        print("off")
