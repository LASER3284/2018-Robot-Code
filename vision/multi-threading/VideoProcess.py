from threading import Thread
#from multiprocessing import Process
import numpy as np
import cv2
import cv2 as cv
from CountsPerSec import CountsPerSec

class VideoProcess:
    '''
    Class that continuously shows a frame using a dedicated thread.
    '''
    def __init__(self, gui, frame=None, resolution=None, trackbarValues=None):
        # Create object pointer for speed tracking.
        self.cps = CountsPerSec().start()

        # Create global variables.
        self.gui = gui
        self.mode = "free"
        self.frame = frame
        self.rotation = 0
        self.processedFrame = frame
        self.objectDetected = False
        self.resolution = resolution
        self.speed = self.cps.countsPerSec()
        self.trackbarValues = trackbarValues
        self.pointArray = [[0,0], [0,0], [0,0], [0,0]]
        self.stopped = False

    def start(self):
        Thread(target=self.detectObject, args=()).start()
        return self

    def detectObject(self):
        # Create variables.
        setPosition = True
        sameAsBefore = 0

        if self.gui == "yes":
            # This is Mostly Useless
            def nothing(x):
                pass

            # Create GUI windows
            cv2.namedWindow("HueComp")
            cv2.namedWindow("SatComp")
            cv2.namedWindow("ValComp")
            cv2.namedWindow("Tracking")
            cv2.createTrackbar("hmin", "HueComp", int(self.trackbarValues[0][0]), 179, nothing)
            cv2.createTrackbar("hmax", "HueComp", int(self.trackbarValues[0][1]), 179, nothing)
            cv2.createTrackbar("smin", "SatComp", int(self.trackbarValues[1][0]), 255, nothing)
            cv2.createTrackbar("smax", "SatComp", int(self.trackbarValues[1][1]), 255, nothing)
            cv2.createTrackbar("vmin", "ValComp", int(self.trackbarValues[2][0]), 255, nothing)
            cv2.createTrackbar("vmax", "ValComp", int(self.trackbarValues[2][1]), 255, nothing)

        while not self.stopped:
            kernel = np.ones((5,5),np.uint8)

            # Create variables for calculations.
            frame = self.frame

            # Take input from camera and split into three windows.
            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            hue,sat,val = cv2.split(hsv)

            # Read and change trackbar values.
            if self.gui == "yes":
                # Determine if trackbars need to be set.
                if self.mode == "free" and sameAsBefore == 0:
                    setPosition = True
                    sameAsBefore = 1

                if self.mode =="tape" and sameAsBefore == 1:
                    setPosition = True
                    sameAsBefore = 0

                # Set trackbar positions when mode changes.
                if setPosition == True:
                    cv2.setTrackbarPos("hmin", "HueComp", int(self.trackbarValues[0][0]))
                    cv2.setTrackbarPos("hmax", "HueComp", int(self.trackbarValues[0][1]))
                    cv2.setTrackbarPos("smin", "SatComp", int(self.trackbarValues[1][0]))
                    cv2.setTrackbarPos("smax", "SatComp", int(self.trackbarValues[1][1]))
                    cv2.setTrackbarPos("vmin", "ValComp", int(self.trackbarValues[2][0]))
                    cv2.setTrackbarPos("vmax", "ValComp", int(self.trackbarValues[2][1]))
                    setPosition = False

                # Get trackbar positions.
                hmn = cv2.getTrackbarPos("hmin","HueComp")
                hmx = cv2.getTrackbarPos("hmax","HueComp")
                smn = cv2.getTrackbarPos("smin","SatComp")
                smx = cv2.getTrackbarPos("smax","SatComp")
                vmn = cv2.getTrackbarPos("vmin","ValComp")
                vmx = cv2.getTrackbarPos("vmax","ValComp")
            else:
                hmn = int(self.trackbarValues[0][0])
                hmx = int(self.trackbarValues[0][1])
                smn = int(self.trackbarValues[1][0])
                smx = int(self.trackbarValues[1][1])
                vmn = int(self.trackbarValues[2][0])
                vmx = int(self.trackbarValues[2][1])

            # Apply thresholding to windows.
            hthresh = cv2.inRange(np.array(hue),np.array(hmn),np.array(hmx))
            sthresh = cv2.inRange(np.array(sat),np.array(smn),np.array(smx))
            vthresh = cv2.inRange(np.array(val),np.array(vmn),np.array(vmx))

            # AND together hue, sat, and val.
            combined = cv2.bitwise_and(hthresh,cv2.bitwise_and(sthresh,vthresh))
            # Remove small blobs in combined image.
            clean = cv2.medianBlur(combined, 5)
            
            # Fuse the image and add slight blur to improve tracking.
            dilation = cv2.dilate(clean, kernel, iterations = 2)
            closing = cv2.morphologyEx(dilation, cv2.MORPH_CLOSE, kernel)
            closing = cv2.GaussianBlur(closing,(5,5),0)

            # Combine all HSV windows to 'closing'
            kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (7, 7))
            # Close any gaps to complete rectangle and output in new window 'closed'
            closed = cv2.morphologyEx(closing, cv2.MORPH_CLOSE, kernel)
            # Create a wireframe from 'closed' and output as 'edged.'
            edged = cv2.Canny(closed, 10, 240)

            # Set new threshold.
            _, bin = cv2.threshold(edged, 40, 255, 0)

            # Find contours of the second image.
            bin, contours, hierarchy = cv2.findContours(bin, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)

            # Read contours and draw the four points of rectangle.
            if len(contours) != 0:
                # Output that program has detected object.
                self.objectDetected = True

                # Find the biggest area.
                c = max(contours, key = cv2.contourArea)
                rect = cv2.minAreaRect(c)
                box = cv2.boxPoints(rect)
                self.pointArray = box
                
                # Get the angle of the rectangle.
                self.rotation = rect[2]

                if self.gui == "yes":
                    # Draw circles and numbers.
                    nameStepper = 0
                    for p in box:
                        nameStepper = nameStepper + 1
                        # Draw blue circles and numbers.
                        pt = int(p[0]), int(p[1])
                        cv2.circle(frame, pt, 5, (255, 0, 0), 2)
                        font = cv2.FONT_HERSHEY_SIMPLEX
                        cv2.putText(frame, str(nameStepper), pt, font, 1, (0, 0, 255), 1, cv2.LINE_AA)

                if self.gui == "no":
                    # Draw enclosing circle.
                    cv2.drawContours(frame, contours, -1, ( 255, 255, 210), 1)
                    (x, y), radius = cv2.minEnclosingCircle(c)
                    cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 1)
                    nameStepper = 0
                    for p in box:
                        # Draw blue circles.
                        pt = int(p[0]), int(p[1])
                        cv2.circle(frame, pt, 5, (255, 0, 0), 1)

            else:
                # Default values if nothing is detected.
                self.pointArray[0][0] = (self.resolution[0] / 2) - 4
                self.pointArray[0][1] = (self.resolution[1] / 2) - 4
                self.pointArray[1][0] = (self.resolution[0] / 2) - 4
                self.pointArray[1][1] = (self.resolution[1] / 2) + 4
                self.pointArray[2][0] = (self.resolution[0] / 2) + 4
                self.pointArray[2][1] = (self.resolution[1] / 2) - 4
                self.pointArray[3][0] = (self.resolution[0] / 2) + 4
                self.pointArray[3][1] = (self.resolution[1] / 2) + 4
                # Output that the program does not detect an object.
                self.objectDetected = False

            # Enable/Disable GUI.
            if self.gui == "yes":
                # Write image output to windows.
                cv2.imshow("HueComp", hthresh)
                cv2.imshow("SatComp", sthresh)
                cv2.imshow("ValComp", vthresh)
                cv2.imshow("Combined", combined)
                cv2.imshow("Clean", clean)
                cv2.imshow("Dilation", dilation)
                cv2.imshow("Closing", closing)
                cv2.imshow("Closed", closed)
                cv2.imshow("Edged", edged)
                cv2.imshow("Tracking", frame)

            # Send the final frame for streaming.
            self.processedFrame = frame

            # Increment counts per second.
            self.cps.increment()
            self.speed = self.cps.countsPerSec()

            # Add small delay and if q is pressed quit.
            if cv2.waitKey(1) == ord("q"):
                self.stopped = True

    def stop(self):
        self.stopped = True