from threading import Thread
import numpy
import time
import cv2

class VideoGet:
    '''
    Class that continuously gets frames from a VideoGet object
    with a dedicated thread.
    '''
    def __init__(self, src=0):
        # Create global variables.
        self.mode = "free"
        self.stream = cv2.VideoCapture(src)
        self.stream.set(3, 300)
        self.stream.set(4, 350)
        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False

    def start(self):
        Thread(target=self.get, args=()).start()
        return self

    def get(self):
        # Grab frame from video stream.
        while not self.stopped:
            if not self.grabbed:
                self.stop()
            else:
                # Adjust brightness for tracking modes.
                if self.mode == "free":
                    self.stream.set(cv2.CAP_PROP_BRIGHTNESS, 0.8)
                if self.mode == "tape":
                    self.stream.set(cv2.CAP_PROP_BRIGHTNESS, 0.05)

                # Read video stream.
                (self.grabbed, self.frame) = self.stream.read()

            # Slow reading of frames because it's to fast.
            time.sleep(0.015)

    def stop(self):
        self.stopped = True