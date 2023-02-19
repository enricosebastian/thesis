import RPi.GPIO as GPIO
import cv2

cap = cv2.VideoCapture(0)

fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
writer = cv2.VideoWriter("recording.mp4", fourcc, 24.0, (640, 480))

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
      print(f"Recaording: {recording}")
      
cap.release()
writer.release()
cv2.destroyAllWindows()

# webcam = cv2.VideoCapture(0)
# video = VideoWriter('webcam.avi', VideoWriter_fourcc(*'MP42'), 24.0, (640, 480))

# while True:
#     stream_ok, frame = webcam.read()
    
#     if stream_ok:
#       cv2.imshow('Webcam rn', frame)
#       video.write(frame)
    
#     if cv2.waitKey(0) and 0xFF == ord('q'):
#         break

# cv2.destroyAllWindows()
# webcam.release()
# video.release()

# offPin = 11

# GPIO.setmode(GPIO.BOARD)
# GPIO.setup(offPin, GPIO.IN)

# while True:
#     if GPIO.input(offPin) == True:
#         print("on")
#     else:
#         print("off")


