#!/usr/bin/env python
import freenect
import cv
import frame_convert
import numpy as np

class Tracker(Object):

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

  def show_depth(self):
    depth, timestamp = freenect.sync_get_depth()
    depth = 255 * np.logical_and(depth >= self.current_depth - self.threshold,
                                 depth <= self.current_depth + self.threshold)
    depth = depth.astype(np.uint8)

    image = cv.CreateImageHeader((depth.shape[1], depth.shape[0]),
                                 cv.IPL_DEPTH_8U,
                                 1)
    cv.SetData(image, depth.tostring(),
               depth.dtype.itemsize * depth.shape[1])

    eig = cv.CreateImage(cv.GetSize(image), 8, 1)
    temp = cv.CreateImage(cv.GetSize(image), 8, 1)

    features = cv.GoodFeaturesToTrack(image, eig, temp, 10, 0.01, 10, None, 3, 0, 0.04)

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
      cv.Circle(image, (x,250), 3, (128, 256, 128), -1, 8, 0)
      print x

    cv.ShowImage('Depth', image)
    self.lx = x
    self.ly = y

    cv.NamedWindow('Depth')
    cv.CreateTrackbar('threshold', 'Depth', self.threshold,     500,  change_threshold)
    cv.CreateTrackbar('depth',     'Depth', self.current_depth, 2048, change_depth)

    print('Press ESC in window to stop')

  #don't run
  def run(self):
    while 1:
      show_depth()
      if cv.WaitKey(10) == 27:
        break
