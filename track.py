#!/usr/bin/env python
import freenect
import cv
import frame_convert
import numpy as np

#values set based on fixed kinect location
#depth of "gesture area"
threshold = 13
#distance from kinect to "gesture area" while sitting on couch
current_depth = 865
#old = np.empty_like(freenect.sync_get_depth()[0].astype(np.uint8))
lx = 0
ly = 0
jitter = 15

def change_threshold(value):
    global threshold
    threshold = value


def change_depth(value):
    global current_depth
    current_depth = value


def show_depth():
    global threshold
    global current_depth
    global old
    global lx
    global ly
    global jitter

    depth, timestamp = freenect.sync_get_depth()
    depth = 255 * np.logical_and(depth >= current_depth - threshold,
                                 depth <= current_depth + threshold)
    depth = depth.astype(np.uint8)

    #if old != None:
      #temp = depth
      #depth = np.absolute(np.subtract(depth, old))
      #old = temp

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
      if abs(x-lx) < jitter:
        x = lx
      if abs(y-ly) < jitter:
        y = ly
      cv.Circle(image, (x,250), 3, (128, 256, 128), -1, 8, 0)
      print x

    cv.ShowImage('Depth', image)
    lx = x
    ly = y

cv.NamedWindow('Depth')
cv.CreateTrackbar('threshold', 'Depth', threshold,     500,  change_threshold)
cv.CreateTrackbar('depth',     'Depth', current_depth, 2048, change_depth)

print('Press ESC in window to stop')


while 1:
    show_depth()
    if cv.WaitKey(10) == 27:
        break
