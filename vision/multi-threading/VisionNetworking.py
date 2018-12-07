from threading import Thread
import logging
import numpy
import time
import cv2
from networktables import NetworkTables

class VisionNetworking:
    '''
    Class that continuously sends data to and from the VideoProcess class
    between the RoboRIO with a dedicated thread.
    '''
    def __init__(self, server, pointArray):
        # Create global variables.
        self.mode = "free"
        self.server = server
        self.pointArray = pointArray
        self.trackbarValues = [[0,0], [0,0], [0,0]]
        self.stopped = False

        # Initialize NetworkTables
        ip = self.server
        NetworkTables.initialize(server = ip)
        self.NWTB = NetworkTables.getTable("SmartDashboard")

        # Send vision NetworkTable data.
        self.NWTB.putBoolean("VisionFreeMode", True)
        self.NWTB.putNumber("VisionObjectSize", 0)
        self.NWTB.putNumber("VisionObjectCenter", 0)
        self.freeModeEnabled = self.NWTB.getEntry("VisionFreeMode")

    def start(self):
        Thread(target=self.network, args=()).start()
        return self

    def network(self):
        # Create variables.
        rotationSwitcher = 0
        trackbarSwitcher = 0
        modeSwitcher = 0

        while not self.stopped:
            # Get NetworkTable data.
            if self.freeModeEnabled.value == True and modeSwitcher == 0:
                self.mode = "free"
                modeSwitcher = 1
            if self.freeModeEnabled.value == False and modeSwitcher == 1:
                self.mode = "tape"
                modeSwitcher = 0

            # Determine rotation of the rectangle.
            if (self.pointArray[0][0]) < (self.pointArray[2][0]):
                rotationSwitcher = 0

            if (self.pointArray[0][0]) > (self.pointArray[2][0]):
                rotationSwitcher = 1

            # Correct calculations depending on orientation.
            if rotationSwitcher == 0:
                # Calculate width of the object.
                xSize = abs(abs((self.pointArray[2][0])) - abs((self.pointArray[0][0])))
                # Find center point on x axis.
                xCenter = abs((self.pointArray[0][0])) + xSize / 2
                # Output values to NetworkTables for robot movement.
                self.NWTB.putNumber("VisionObjectSize", xSize)
                self.NWTB.putNumber("VisionObjectCenter", xCenter)

            if rotationSwitcher == 1:
                # Calculate the width of the object with reversed calculations.
                xSize = abs(abs((self.pointArray[0][0])) - abs((self.pointArray[2][0])))
                # Find center point on x axis.
                xCenter = abs((self.pointArray[2][0])) + xSize / 2
                # Output values to NetworkTables for robot movement.
                self.NWTB.putNumber("VisionObjectSize", xSize)
                self.NWTB.putNumber("VisionObjectCenter", xCenter)

            # Open stored trackbar values.
            if self.mode == "free" and trackbarSwitcher == 0:
                hmnReadFree = open('/home/vision/Desktop/Values/FreeMode/hmn.txt', 'r')
                (self.trackbarValues[0][0]) = hmnReadFree.read()
                hmxReadFree = open('/home/vision/Desktop/Values/FreeMode/hmx.txt', 'r')
                (self.trackbarValues[0][1]) = hmxReadFree.read()
                smnReadFree = open('/home/vision/Desktop/Values/FreeMode/smn.txt', 'r')
                (self.trackbarValues[1][0]) = smnReadFree.read()
                smxReadFree = open('/home/vision/Desktop/Values/FreeMode/smx.txt', 'r')
                (self.trackbarValues[1][1]) = smxReadFree.read()
                vmnReadFree = open('/home/vision/Desktop/Values/FreeMode/vmn.txt', 'r')
                (self.trackbarValues[2][0]) = vmnReadFree.read()
                vmxReadFree = open('/home/vision/Desktop/Values/FreeMode/vmx.txt', 'r')
                (self.trackbarValues[2][1]) = vmxReadFree.read()
                trackbarSwitcher = 1

            if self.mode == "tape" and trackbarSwitcher == 1:
                hmnReadTape = open('/home/vision/Desktop/Values/TapeMode/hmn.txt', 'r')
                (self.trackbarValues[0][0]) = hmnReadTape.read()
                hmxReadTape = open('/home/vision/Desktop/Values/TapeMode/hmx.txt', 'r')
                (self.trackbarValues[0][1]) = hmxReadTape.read()
                smnReadTape = open('/home/vision/Desktop/Values/TapeMode/smn.txt', 'r')
                (self.trackbarValues[1][0]) = smnReadTape.read()
                smxReadTape = open('/home/vision/Desktop/Values/TapeMode/smx.txt', 'r')
                (self.trackbarValues[1][1]) = smxReadTape.read()
                vmnReadTape = open('/home/vision/Desktop/Values/TapeMode/vmn.txt', 'r')
                (self.trackbarValues[2][0]) = vmnReadTape.read()
                vmxReadTape = open('/home/vision/Desktop/Values/TapeMode/vmx.txt', 'r')
                (self.trackbarValues[2][1]) = vmxReadTape.read()
                trackbarSwitcher = 0

            # Add delay so it doesn't flood the network.
            time.sleep(0.02)

    def stop(self):
        self.stopped = True