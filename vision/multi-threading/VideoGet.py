from threading import Thread
import numpy
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
        self.stream.set(3, 180)                            # 300->288 | 100->120
        self.stream.set(4, 240)                            # 400->352 | 100->160
        (self.grabbed, self.frame) = self.stream.read()
        self.resolution = [self.stream.get(3), self.stream.get(4)]
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
                    self.stream.set(cv2.CAP_PROP_BRIGHTNESS, 0)
                if self.mode == "tape":
                    self.stream.set(cv2.CAP_PROP_BRIGHTNESS, 0.05)

                # Read video stream.
                (self.grabbed, self.frame) = self.stream.read()

    def stop(self):
        self.stopped = True