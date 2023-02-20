import RPi.GPIO as GPIO
import cv2
import datetime

# cap = cv2.VideoCapture(0)

# fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
# writer = cv2.VideoWriter("recording.mp4", fourcc, 24.0, (640, 480))

recording = False

while True:
   print(datetime.datetime().microsecond())
#    ret, frame = cap.read()
   
#    if ret:
#       cv2.imshow('Video recording', frame)
#       if recording:
#          writer.write(frame)
         
#    key = cv2.waitKey(1)
   
#    if key == ord('q'):
#       break
#    elif key == ord('r'):
#       recording = not recording
#       print(f"Recaording: {recording}")
      
# cap.release()
# writer.release()
# cv2.destroyAllWindows()