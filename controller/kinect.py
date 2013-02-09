#!/usr/bin/env python
import freenect
import cv
import frame_convert
import numpy as np

class KinectTracker:

  def __init__(self):
    #values set based on fixed kinect location
    #depth of "gesture area"
    self.threshold = 8
    #distance from kinect to "gesture area" while sitting on couch
    self.current_depth = 994
    #keep track of old values
    self.lx = 0
    self.ly = 0
    #thresholded to stop jittering the ghetto way
    self.jitter = 15

  def change_threshold(self, value):
    self.threshold = value

  def change_depth(self, value):
    self.current_depth = value

  def change_jitter(self, value):
    self.jitter = value

  def get_position(self):
    return (self.lx < 160, self.ly > 160)

  def update(self):
    depth, timestamp = freenect.sync_get_depth()
    depth = 255 * np.logical_and(depth >= self.current_depth - self.threshold,
                                 depth <= self.current_depth + self.threshold)
    depth = depth.astype(np.uint8)

    image = cv.CreateImageHeader((depth.shape[1], depth.shape[0]), cv.IPL_DEPTH_8U, 1)
    cv.SetData(image, depth.tostring(), depth.dtype.itemsize * depth.shape[1])

    cropped = cv.GetSubRect(image, (200, 75, 240, 200))

    features = cv.GoodFeaturesToTrack(cropped, None, None, 7, 0.01, 10, None, 3, 0, 0.04)

    #shitty not python code
    i = 0
    x = 0
    y = 0
    for (a,b) in features:
      i += 1
      x += a
      y += b

    if i != 0:
      if abs(x-self.lx) < self.jitter or x == 0:
        x = self.lx
      if abs(y-self.ly) < self.jitter or y == 0:
        y = self.ly

    self.lx = x
    self.ly = y

    return cropped

  def show_update(self):
    image = self.update()
    cv.Circle(image, (self.lx, self.ly), 3, (128, 256, 128), -1, 8, 0)
    cv.ShowImage('Depth', image)

if __name__ == '__main__':
  while 1:
    t = KinectTracker();
    cv.NamedWindow('Depth')
    cv.CreateTrackbar('threshold', 'Depth', t.threshold,     500,  t.change_threshold)
    cv.CreateTrackbar('depth',     'Depth', t.current_depth, 2048, t.change_depth)
    t.show_update()
    if cv.WaitKey(10) == 27:
      break
