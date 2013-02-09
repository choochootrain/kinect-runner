import kinect
import time

controller = kinect.KinectTracker()

while True:
  t = time.time()
  controller.update()
  print time.time() - t
  print controller.get_position()
