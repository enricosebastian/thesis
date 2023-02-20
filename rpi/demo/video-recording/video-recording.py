import RPi.GPIO as GPIO
import cv2
import datetime
import os

now = datetime.datetime.now()
now_string = now.strftime("%H-%M-%S")
filename = now_string+".mp4"

cap = cv2.VideoCapture(0)

fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
writer = cv2.VideoWriter(filename, fourcc, 24.0, (640, 480))

recording = False

while True:
   ret, frame = cap.read()
   
   if ret:
      cv2.imshow('Video recording', frame)
      if recording:
         writer.write(frame)
         
   key = cv2.waitKey(1)
   
   if key == ord('q'):
      break
   elif key == ord('r'):
      recording = not recording
      print(f"Recording: {recording}")
      
cap.release()
writer.release()
cv2.destroyAllWindows()