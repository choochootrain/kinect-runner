#!/usr/bin/env python
import freenect
import cv
import frame_convert
import numpy as np

class KinectTracker:

  def __init__(self):
    #values set based on fixed kinect location
    #depth of "gesture area"
    self.threshold = 13
    #distance from kinect to "gesture area" while sitting on couch
    self.current_depth = 865
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
    return (self.lx, self.ly)

  def update(self):
    depth, timestamp = freenect.sync_get_depth()
    depth = 255 * np.logical_and(depth >= self.current_depth - self.threshold,
                                 depth <= self.current_depth + self.threshold)
    depth = depth.astype(np.uint8)

    image = cv.CreateImageHeader((depth.shape[1], depth.shape[0]), cv.IPL_DEPTH_8U, 1)
    cv.SetData(image, depth.tostring(), depth.dtype.itemsize * depth.shape[1])

    features = cv.GoodFeaturesToTrack(image, None, None, 7, 0.01, 10, None, 3, 0, 0.04)

    #shitty not python code
    i = 0
    x = 0
    y = 0
    for (a,b) in features:
      i += 1
      x += a
      y += b

    if i != 0:
      x = int(x/i)
      y = int(y/i)
      if abs(x-self.lx) < self.jitter:
        x = self.lx
      if abs(y-self.ly) < self.jitter:
        y = self.ly

    self.lx = x
    self.ly = y

    return image

  def show_update(self):
    image = self.update()
    cv.Circle(image, (self.lx, self.ly), 3, (128, 256, 128), -1, 8, 0)
    cv.ShowImage('Depth', image)
    cv.NamedWindow('Depth')
    cv.CreateTrackbar('threshold', 'Depth', self.threshold,     500,  self.change_threshold)
    cv.CreateTrackbar('depth',     'Depth', self.current_depth, 2048, self.change_depth)

if __name__ == '__main__':
  while 1:
    t = KinectTracker();
    t.show_update()
    if cv.WaitKey(10) == 27:
      break
