#from multiprocessing import Process
from threading import Thread
import logging
import numpy
import math
import time
import cv2
from networktables import NetworkTables

class VisionNetworking:
    '''
    Class that continuously sends data to and from the VideoProcess class
    between the RoboRIO with a dedicated thread.
    '''
    def __init__(self, gui, server, pointArray, objectDetected, rotation, resolution):
        # Create global variables.
        self.gui = gui
        self.mode = "free"
        self.speed = 0
        self.server = server
        self.rotation = rotation
        self.pointArray = pointArray
        self.objectDetected = objectDetected
        self.resolution = resolution
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
        self.NWTB.putNumber("VisionHorizontalRes", self.resolution[0])
        self.freeModeEnabled = self.NWTB.getEntry("VisionFreeMode")
        
        # Open values from files and set trackbar position.
        self.hmnReadFree = open("/home/pi/Desktop/Values/FreeMode/hmn.txt", "r")
        self.hmxReadFree = open("/home/pi/Desktop/Values/FreeMode/hmx.txt", "r")
        self.smnReadFree = open("/home/pi/Desktop/Values/FreeMode/smn.txt", "r")
        self.smxReadFree = open("/home/pi/Desktop/Values/FreeMode/smx.txt", "r")
        self.vmnReadFree = open("/home/pi/Desktop/Values/FreeMode/vmn.txt", "r")
        self.vmxReadFree = open("/home/pi/Desktop/Values/FreeMode/vmx.txt", "r")
        (self.trackbarValues[0][0]) = self.hmnReadFree.read()
        (self.trackbarValues[0][1]) = self.hmxReadFree.read()
        (self.trackbarValues[1][0]) = self.smnReadFree.read()
        (self.trackbarValues[1][1]) = self.smxReadFree.read()
        (self.trackbarValues[2][0]) = self.vmnReadFree.read()
        (self.trackbarValues[2][1]) = self.vmxReadFree.read()

    def start(self):
        Thread(target=self.network, args=()).start()
        return self

    def network(self):
        # Create variables and object pointers.
        rotationSwitcher = 0
        modeSwitcher = 0
        oldTime = 0
        
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
                # Print debug info if the gui is disabled.
                if self.gui == "no":
                    print "Object Rotation: normal"

            if rotationSwitcher == 1:
                # Calculate the width of the object with reversed calculations.
                xSize = abs(abs((self.pointArray[0][0])) - abs((self.pointArray[2][0])))
                # Find center point on x axis.
                xCenter = abs((self.pointArray[2][0])) + xSize / 2
                # Output values to NetworkTables for robot movement.
                self.NWTB.putNumber("VisionObjectSize", xSize)
                self.NWTB.putNumber("VisionObjectCenter", xCenter)
                # Print debug info if the gui is disabled.
                if self.gui == "no":
                    print "Object Rotation: rotated"

            # Check if the files or mode have changed and set trackbar values.
            if self.mode == "free":
                hmnReadFree = open("/home/pi/Desktop/Values/FreeMode/hmn.txt", "r")
                if (self.trackbarValues[0][0]) != hmnReadFree.read():
                    (self.trackbarValues[0][0]) = hmnReadFree.read()
                hmxReadFree = open("/home/pi/Desktop/Values/FreeMode/hmx.txt", "r")
                if (self.trackbarValues[0][1]) != hmxReadFree.read():
                    (self.trackbarValues[0][1]) = hmxReadFree.read()
                smnReadFree = open("/home/pi/Desktop/Values/FreeMode/smn.txt", "r")
                if (self.trackbarValues[1][0]) != smnReadFree.read():
                    (self.trackbarValues[1][0]) = smnReadFree.read()
                smxReadFree = open("/home/pi/Desktop/Values/FreeMode/smx.txt", "r")
                if (self.trackbarValues[1][1]) != smxReadFree.read():
                    (self.trackbarValues[1][1]) = smxReadFree.read()
                vmnReadFree = open("/home/pi/Desktop/Values/FreeMode/vmn.txt", "r")
                if (self.trackbarValues[2][0]) != vmnReadFree.read():
                    (self.trackbarValues[2][0]) = vmnReadFree.read()
                vmxReadFree = open("/home/pi/Desktop/Values/FreeMode/vmx.txt", "r")
                if (self.trackbarValues[2][1]) != vmxReadFree.read():
                    (self.trackbarValues[2][1]) = vmxReadFree.read()

            if self.mode == "tape":
                hmnReadTape = open("/home/pi/Desktop/Values/TapeMode/hmn.txt", "r")
                if (self.trackbarValues[0][0]) != hmnReadTape.read():
                    (self.trackbarValues[0][0]) = hmnReadTape.read()
                hmxReadTape = open("/home/pi/Desktop/Values/TapeMode/hmx.txt", "r")
                if (self.trackbarValues[0][1]) != hmxReadTape.read():
                    (self.trackbarValues[0][1]) = hmxReadTape.read()
                smnReadTape = open("/home/pi/Desktop/Values/TapeMode/smn.txt", "r")
                if (self.trackbarValues[1][0]) != smnReadTape.read():
                    (self.trackbarValues[1][0]) = smnReadTape.read()
                smxReadTape = open("/home/pi/Desktop/Values/TapeMode/smx.txt", "r")
                if (self.trackbarValues[1][1]) != smxReadTape.read():
                    (self.trackbarValues[1][1]) = smxReadTape.read()
                vmnReadTape = open("/home/pi/Desktop/Values/TapeMode/vmn.txt", "r")
                if (self.trackbarValues[2][0]) != vmnReadTape.read():
                    (self.trackbarValues[2][0]) = vmnReadTape.read()
                vmxReadTape = open("/home/pi/Desktop/Values/TapeMode/vmx.txt", "r")
                if (self.trackbarValues[2][1]) != vmxReadTape.read():
                    (self.trackbarValues[2][1]) = vmxReadTape.read()

            # Send telemtry data to webserver.
            seconds = int(time.time())
            if seconds % 2 == 0 and seconds != oldTime:
                telemetryMode = open("/home/pi/Desktop/Values/Telemetry/mode.txt", "w")
                telemetryMode.write(self.mode)
                telemetrySpeed = open("/home/pi/Desktop/Values/Telemetry/speed.txt", "w")
                telemetrySpeed.write(str(self.speed))
                telemetryPointArray = open("/home/pi/Desktop/Values/Telemetry/pointarray.txt", "w")
                telemetryPointArray.write(str(self.pointArray))
                telemetryTrackbars = open("/home/pi/Desktop/Values/Telemetry/trackbarvalues.txt", "w")
                telemetryTrackbars.write(str(self.trackbarValues))
                oldTime = seconds

            # Print debug info if the gui is disabled.
            if self.gui == "no":
                print "Vision Camera Res: " + str(self.resolution[0]) + "x" + str(self.resolution[1])
                print "Vision Mode: " + self.mode
                print "Vision Size: " + str(xSize)
                print "Vision Center: " + str(xCenter)
                print "Vision Rotation: " + str(self.rotation)
                print "Object Detected: " + str(self.objectDetected)
                print "Vision Frequency: " + str(self.speed)
                print "Vision Point Array: \n" + str(self.pointArray)
                print "Vision Track-bar Values Array: " + str(self.trackbarValues)

            # Add delay so it doesn't flood the network.
            time.sleep(0.07) # 0.02s

    def stop(self):
        self.stopped = True