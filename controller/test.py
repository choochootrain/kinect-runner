import kinect

controller = kinect.KinectTracker()

while True:
  controller.update()
  print controller.get_position()
