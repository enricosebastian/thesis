import RPi.GPIO as GPIO

import cv2
from cv2 import VideoWriter
from cv2 import VideoWriter_fourcc

webcam = cv2.VideoCapture(0)
video = VideoWriter('webcam.avi', VideoWriter_fourcc(*'MP42'), 24.0, (640, 480))

while True:
    stream_ok, frame = webcam.read()
    
    if stream_ok:
      cv2.imshow('Webcam rn', frame)
      video.write(frame)
    
    if cv2.waitKey(0) and 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
webcam.release()
video.release()

# offPin = 11

# GPIO.setmode(GPIO.BOARD)
# GPIO.setup(offPin, GPIO.IN)

# while True:
#     if GPIO.input(offPin) == True:
#         print("on")
#     else:
#         print("off")


