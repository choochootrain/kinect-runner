#!/usr/bin/env python
import freenect
import cv
import frame_convert
import numpy as np

#values set based on fixed kinect location
#depth of "gesture area"
threshold = 8
#distance from kinect to "gesture area" while sitting on couch
current_depth = 994
old = None

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

    depth, timestamp = freenect.sync_get_depth()
    depth = 255 * np.logical_and(depth >= current_depth - threshold,
                                 depth <= current_depth + threshold)
    depth = depth.astype(np.uint8)

    if old != None:
      temp = depth
      depth = np.absolute(np.subtract(depth, old))
      old = temp

    image = cv.CreateImageHeader((depth.shape[1], depth.shape[0]),
                                 cv.IPL_DEPTH_8U,
                                 1)
    cv.SetData(image, depth.tostring(),
               depth.dtype.itemsize * depth.shape[1])
    cv.ShowImage('Depth', image)


cv.NamedWindow('Depth')
cv.CreateTrackbar('threshold', 'Depth', threshold,     500,  change_threshold)
cv.CreateTrackbar('depth',     'Depth', current_depth, 2048, change_depth)

print('Press ESC in window to stop')


while 1:
    show_depth()
    if cv.WaitKey(10) == 27:
        break
