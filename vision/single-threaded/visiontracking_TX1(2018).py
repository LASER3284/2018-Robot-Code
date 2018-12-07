###################################################################################################
					
                    					###INIT PROCESS##

###################################################################################################

#from Tkinter import *
import cv2
import cv2 as cv
import numpy as np
import time
import sys
import logging
from networktables import NetworkTables

kernel = np.ones((5,5),np.uint8)
 
# Take input from webcam
cap = cv2.VideoCapture(0)
 
# Reduce the size of video to 400x300 (3:4) so CPU can process faster (real resolution is actually 351x287).
cap.set(3,300)
cap.set(4,400)
 
def nothing(x):
    pass

# Creating windows for later blending and viewing.
cv2.namedWindow('HueComp')
cv2.namedWindow('SatComp')
cv2.namedWindow('ValComp')
#cv2.namedWindow('closing')
#cv2.namedWindow('closed')
cv2.namedWindow('tracking')
cv2.namedWindow('edged')

# Creating tracking bars for hue, saturation and value windows. 
cv2.createTrackbar('hmin', 'HueComp',24,179,nothing)
cv2.createTrackbar('hmax', 'HueComp',24,179,nothing)
 
cv2.createTrackbar('smin', 'SatComp',24,255,nothing)
cv2.createTrackbar('smax', 'SatComp',24,255,nothing)
 
cv2.createTrackbar('vmin', 'ValComp',24,255,nothing)
cv2.createTrackbar('vmax', 'ValComp',24,255,nothing)

###############################################################################

###############################################################################
                ###INITIALIZE NETWORK TABLES###
###############################################################################
#Ask for IP Address and if not provided then give error.
if len(sys.argv) != 2:
    print("Error: specify an IP to connect to!")
    exit(0)

ip = sys.argv[1]
NetworkTables.initialize(server=ip)

#Some logging stuff.
logging.basicConfig(level=logging.DEBUG)
###############################################################################

###############################################################################
                ###SETUP NETWORKTABLES FOR X AND Y###
###############################################################################
NWTB = NetworkTables.getTable("SmartDashboard")
############################################################################### 

###############################################################################
	###SETUP NETWORKTABLES FOR MODE SWITCHING###
###############################################################################
#Set up mode switching for cubes or reflective tape.
NWTBMODE = NetworkTables.getTable("SmartDashboard")
NWTBMODE.putBoolean('VisionFreeMode', True)
#Create variables for mode setting.
isFreeMode = NWTBMODE.getEntry('VisionFreeMode')
isSameAsBefore = int(0)
counter = int(0)
###############################################################################

###############################################################################
		###SETUP NETWORKTABLES FOR TRACKBARS###
###############################################################################
NWTBTRACKBAR = NetworkTables.getTable("SmartDashboard")
trackbarCounter = int(0)
###############################################################################

###############################################################################
	###SETUP NETWORKTABLES FOR TRACKING DISTANCE AND POSITION###
###############################################################################
NWTBLOCATION = NetworkTables.getTable("SmartDashboard")
NWTBLOCATION.putNumber('LockObjectThresh', 135)
NWTB.putNumber('Vision_Point1x', 1)
NWTB.putNumber('Vision_Point3x', 1)
xSize = int(0)
xCenter = int(0)
switcher = int(0)
###############################################################################

####################################################################################################


####################################################################################################
				
					###MAIN PROCESS###

####################################################################################################

while(1):
 
    buzz = 0

    #If the TapeMode is enabled then decrease image brightness if not return to normal.
    if isFreeMode.value == False:
        cap.set(cv2.CAP_PROP_BRIGHTNESS, 0.08)
    if isFreeMode.value == True:
        cap.set(cv2.CAP_PROP_BRIGHTNESS, 0.1)

    _, frame = cap.read()

###############################################################################
		###SET THE TRACKING MODE###
###############################################################################    
    #Open txt files to read for FreeMode and link variables.
    hmnReadFree = open('Values/FreeMode/hmn.txt', 'r')
    hmnValueFree = hmnReadFree.read()
    hmxReadFree = open('Values/FreeMode/hmx.txt', 'r')
    hmxValueFree = hmxReadFree.read()
    smnReadFree = open('Values/FreeMode/smn.txt', 'r')
    smnValueFree = smnReadFree.read()
    smxReadFree = open('Values/FreeMode/smx.txt', 'r')
    smxValueFree = smxReadFree.read()
    vmnReadFree = open('Values/FreeMode/vmn.txt', 'r')
    vmnValueFree = vmnReadFree.read()
    vmxReadFree = open('Values/FreeMode/vmx.txt', 'r')
    vmxValueFree = vmxReadFree.read()
    
    #Get trackbar positions for comparing values.
    hmnPosition = cv2.getTrackbarPos('hmin','HueComp')
    hmxPosition = cv2.getTrackbarPos('hmax','HueComp')
    smnPosition = cv2.getTrackbarPos('smin','SatComp')
    smxPosition = cv2.getTrackbarPos('smax','SatComp')
    vmnPosition = cv2.getTrackbarPos('vmin','ValComp')
    vmxPosition = cv2.getTrackbarPos('vmax','ValComp')

    #Put trackbar values in NetworkTables.
    if trackbarCounter <= 1:
        NWTBTRACKBAR.putNumber('hmn', hmnPosition)
        NWTBTRACKBAR.putNumber('hmx', hmxPosition)
        NWTBTRACKBAR.putNumber('smn', smnPosition)
        NWTBTRACKBAR.putNumber('smx', smxPosition)
        NWTBTRACKBAR.putNumber('vmn', vmnPosition)
        NWTBTRACKBAR.putNumber('vmx', vmxPosition)
        trackbarCounter = trackbarCounter + 1

    #Take mode and set values depending on mode.    
    if isFreeMode.value == True:
       if isSameAsBefore == 0:
           while counter <= 1:
                hmn = cv2.setTrackbarPos('hmin','HueComp', int(hmnValueFree))
                hmx = cv2.setTrackbarPos('hmax','HueComp', int(hmxValueFree))
                smn = cv2.setTrackbarPos('smin','SatComp', int(smnValueFree))
                smx = cv2.setTrackbarPos('smax','SatComp', int(smxValueFree))
                vmn = cv2.setTrackbarPos('vmin','ValComp', int(vmnValueFree))
                vmx = cv2.setTrackbarPos('vmax','ValComp', int(vmxValueFree))
                NWTBTRACKBAR.putNumber('hmn', int(hmnValueFree))
                NWTBTRACKBAR.putNumber('hmx', int(hmxValueFree))
                NWTBTRACKBAR.putNumber('smn', int(smnValueFree))
                NWTBTRACKBAR.putNumber('smx', int(smxValueFree))
                NWTBTRACKBAR.putNumber('vmn', int(vmnValueFree))
                NWTBTRACKBAR.putNumber('vmx', int(vmxValueFree))
                isSameAsBefore = 1
                counter = counter + 1
           counter = 0

    if isFreeMode.value == True:
        hmnFree = hmnPosition
        hmxFree = hmxPosition
        smnFree = smnPosition
        smxFree = smxPosition
        vmnFree = vmnPosition
        vmxFree = vmxPosition
        if str(hmnValueFree) != str(hmnFree):
            hmnWriteFree = open('Values/FreeMode/hmn.txt', 'w')
            test = str(hmnFree)
            hmnWriteFree.write(test)
        if str(hmxValueFree) != str(hmxFree): 
            hmxWriteFree = open('Values/FreeMode/hmx.txt', 'w')
            test = str(hmxFree)
            hmxWriteFree.write(test)
        if str(smnValueFree) != str(smnFree):
            smnWriteFree = open('Values/FreeMode/smn.txt', 'w')
            test = str(smnFree)
            smnWriteFree.write(test) 
        if str(smxValueFree) != str(smxFree):
            smxWriteFree = open('Values/FreeMode/smx.txt', 'w')
            test = str(smxFree)
            smxWriteFree.write(test)
        if str(vmnValueFree) != str(vmnFree):
            vmnWriteFree = open('Values/FreeMode/vmn.txt', 'w')
            test = str(vmnFree)
            vmnWriteFree.write(test)
        if str(vmxValueFree) != str(vmxFree):
            vmxWriteFree = open('Values/FreeMode/vmx.txt', 'w')
            test = str(vmxFree)
            vmxWriteFree.write(test)

    #Open txt files to read for TapeMode.
    hmnReadTape = open('Values/TapeMode/hmn.txt', 'r')
    hmnValueTape = hmnReadTape.read()
    hmxReadTape = open('Values/TapeMode/hmx.txt', 'r')
    hmxValueTape = hmxReadTape.read()
    smnReadTape = open('Values/TapeMode/smn.txt', 'r')
    smnValueTape = smnReadTape.read()
    smxReadTape = open('Values/TapeMode/smx.txt', 'r')
    smxValueTape = smxReadTape.read()
    vmnReadTape = open('Values/TapeMode/vmn.txt', 'r')
    vmnValueTape = vmnReadTape.read()
    vmxReadTape = open('Values/TapeMode/vmx.txt', 'r')
    vmxValueTape = vmxReadTape.read()

    #Take mode and set values depending on mode.
    if isFreeMode.value == False:
        if isSameAsBefore == 1:
            while counter <= 1:
                hmn = cv2.setTrackbarPos('hmin','HueComp', int(hmnValueTape))
                hmx = cv2.setTrackbarPos('hmax','HueComp', int(hmxValueTape))
                smn = cv2.setTrackbarPos('smin','SatComp', int(smnValueTape))
                smx = cv2.setTrackbarPos('smax','SatComp', int(smxValueTape))
                vmn = cv2.setTrackbarPos('vmin','ValComp', int(vmnValueTape))
                vmx = cv2.setTrackbarPos('vmax','ValComp', int(vmxValueTape))
                NWTBTRACKBAR.putNumber('hmn', int(hmnValueTape))
                NWTBTRACKBAR.putNumber('hmx', int(hmxValueTape))
                NWTBTRACKBAR.putNumber('smn', int(smnValueTape))
                NWTBTRACKBAR.putNumber('smx', int(smxValueTape))
                NWTBTRACKBAR.putNumber('vmn', int(vmnValueTape))
                NWTBTRACKBAR.putNumber('vmx', int(vmxValueTape))
                isSameAsBefore = 0
                counter = counter + 1
            counter = 0
    if isFreeMode.value == False:
        hmnTape = hmnPosition
        hmxTape = hmxPosition
        smnTape = smnPosition
        smxTape = smxPosition
        vmnTape = vmnPosition
        vmxTape = vmxPosition
        if str(hmnValueTape) != str(hmnTape): 
            hmnWriteTape = open('Values/TapeMode/hmn.txt', 'w')
            test = str(hmnTape)
            hmnWriteTape.write(test)
        if str(hmxValueTape) != str(hmxTape):
            hmxWriteTape = open('Values/TapeMode/hmx.txt', 'w')
            test = str(hmxTape)
            hmxWriteTape.write(test)
        if str(smnValueTape) != str(smnTape):
            smnWriteTape = open('Values/TapeMode/smn.txt', 'w')
            test = str(smnTape)
            smnWriteTape.write(test)
        if str(smxValueTape) != str(smxTape):
            smxWriteTape = open('Values/TapeMode/smx.txt', 'w')
            test = str(smxTape)
            smxWriteTape.write(test)
        if str(vmnValueTape) != str(vmnTape):
            vmnWriteTape = open('Values/TapeMode/vmn.txt', 'w')
            test = str(vmnTape)
            vmnWriteTape.write(test)
        if str(vmxValueTape) != str(vmxTape):
            vmxWriteTape = open('Values/TapeMode/vmx.txt', 'w')
            test = str(vmxTape)
            vmxWriteTape.write(test)
###############################################################################

###############################################################################
		###OUTPUT TRACKBAR VALUES TO NETWORK TABLES###
###############################################################################
    #Create variables for getting info grom trackbars.
    NWTBTrackbarValueHmn = NWTBTRACKBAR.getEntry('hmn')
    NWTBTrackbarValueHmx = NWTBTRACKBAR.getEntry('hmx')
    NWTBTrackbarValueSmn = NWTBTRACKBAR.getEntry('smn')
    NWTBTrackbarValueSmx = NWTBTRACKBAR.getEntry('smx')
    NWTBTrackbarValueVmn = NWTBTRACKBAR.getEntry('vmn')
    NWTBTrackbarValueVmx = NWTBTRACKBAR.getEntry('vmx')

    #Link local and network trackbars.
    if hmnPosition != NWTBTrackbarValueHmn.value:
        cv2.setTrackbarPos('hmin','HueComp', int(NWTBTrackbarValueHmn.value))
    if hmxPosition != NWTBTrackbarValueHmx.value:
        cv2.setTrackbarPos('hmax','HueComp', int(NWTBTrackbarValueHmx.value))
    if smnPosition != NWTBTrackbarValueSmn.value:
        cv2.setTrackbarPos('smin','SatComp', int(NWTBTrackbarValueSmn.value))
    if hmxPosition != NWTBTrackbarValueSmx.value:
        cv2.setTrackbarPos('smax','SatComp', int(NWTBTrackbarValueSmx.value))
    if vmnPosition != NWTBTrackbarValueVmn.value:
        cv2.setTrackbarPos('vmin','ValComp', int(NWTBTrackbarValueVmn.value))
    if vmxPosition != NWTBTrackbarValueVmx.value:
        cv2.setTrackbarPos('vmax','ValComp', int(NWTBTrackbarValueVmx.value))	
###############################################################################

###############################################################################
			###ESTIMATE OBJECT DISTANCE AND POSITION###
###############################################################################
    #Get required info for calculations and set variables.
    point1x = NWTB.getEntry("Vision_Point1x")
    point3x = NWTB.getEntry("Vision_Point3x")
    lockObjectThresh = NWTBLOCATION.getEntry('LockObjectThresh')
    position = str('')
    distance = str('')

    #If the rectangle is normal set switcher as a toggle for normal calculations. 
    if point1x.value > 180 and point3x.value < 170:
        switcher = 0
           
    #If the rectangle is rotated set switcher as a toggle for rotated calculations.
    if point1x.value < 170 and point3x.value > 180:
        switcher = 1
    
    #Do calculations depending on switcher toggle.
    if switcher == 0:
       #Find the x distance wide the object is to compare size.
       xSize = abs(abs(point1x.value) - abs(point3x.value))
       #Find center point of object on x axis.
       xCenter = (abs(point3x.value) + (xSize / 2))
       print str('normal')
    if switcher == 1:
       #Find the x distance wide the object is to compare size with reversed calculations.
       xSize = abs(abs(point3x.value) - abs(point1x.value))
       #Find center point of object on x axis with reversed calculations.
       xCenter = (abs(point1x.value) + (xSize / 2))
       print str('rotated')

    #Output the location of the object (Left, Right, Center).
    if xCenter > 200:
        position = 'Right'
        NWTBLOCATION.putString('VisionPosition', 'Right')
        #print position
    if xCenter < 145:
        position = 'Left'
        NWTBLOCATION.putString('VisionPosition', 'Left')
        #print position
    if xCenter >= 145 and xCenter <= 200:
        position = 'Center'
        NWTBLOCATION.putString('VisionPosition', 'Center')
        #print position
    
    #Estimate the distance of the object.
    if xSize >= 10 and xSize < 20:
        distance = "Small Object"
        NWTBLOCATION.putString('VisionDistance', 'Small Object')
    if xSize >= 20 and xSize < 50:
        distance = "Medium-Small Object"
        NWTBLOCATION.putString('VisionDistance', 'Medium-Small Object')
    if xSize >= 50 and xSize < 70:
	distance = "Medium Object"
        NWTBLOCATION.putString('VisionDistance', 'Medium Object')
    if xSize >= 70 and xSize < 100:
	position = "Med-Large Object"
        NWTBLOCATION.putString('VisionDistance', 'Med-Large Object')
    if xSize >= 100 and xSize < 120:
        position = "Large Object"
        NWTBLOCATION.putString('VisionDistance', 'Large Object')
    if xSize >= 120 and xSize < lockObjectThresh.value:
        position = "Max Object"
        NWTBLOCATION.putString('VisionDistance', 'Max Object')
    if xSize >= lockObjectThresh.value:
        position = "Lock Object"
        NWTBLOCATION.putString('VisionDistance', 'Lock Object')
    
    #Quick print values.
    #print str(NWTBLOCATION.getEntry('VisionPosition').value)
    #print xCenter
    #print xSize

    #Print xCenter and xSize to NetworkTables.
    NWTBLOCATION.putNumber('VisionPositionNumber', xCenter)
    NWTBLOCATION.putNumber('VisionDistanceNumber', xSize)
###############################################################################

###############################################################################
    ###START TRACKING AND DOING CALCULATIONS FOR DETECTING RECTANGLES###
###############################################################################
    #Convert input from camera to hue, sat, and val and split it to the windows.
    hsv = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
    hue,sat,val = cv2.split(hsv)

    # Get info from track bar and appy to result.
    hmn = int(NWTBTrackbarValueHmn.value)	#cv2.getTrackbarPos('hmin','HueComp') #REPLACE WITH NETWORKTABLE VALUES
    hmx = int(NWTBTrackbarValueHmx.value)	#cv2.getTrackbarPos('hmax','HueComp') #REPLACE WITH NETWORKTABLE VALUES

    smn = int(NWTBTrackbarValueSmn.value)	#cv2.getTrackbarPos('smin','SatComp') #REPLACE WITH NETWORKTABLE VALUES
    smx = int(NWTBTrackbarValueSmx.value)	#cv2.getTrackbarPos('smax','SatComp') #REPLACE WITH NETWORKTABLE VALUES

    vmn = int(NWTBTrackbarValueVmn.value)	#cv2.getTrackbarPos('vmin','ValComp') #REPLACE WITH NETWORKTABLE VALUES
    vmx = int(NWTBTrackbarValueVmx.value)	#cv2.getTrackbarPos('vmax','ValComp') #REPLACE WITH NETWORKTABLE VALUES
 
    # Apply thresholding to HSV windows.
    hthresh = cv2.inRange(np.array(hue),np.array(hmn),np.array(hmx))
    sthresh = cv2.inRange(np.array(sat),np.array(smn),np.array(smx))
    vthresh = cv2.inRange(np.array(val),np.array(vmn),np.array(vmx))
 
    #AND hue, sat, and val.
    tracking = cv2.bitwise_and(hthresh,cv2.bitwise_and(sthresh,vthresh))
 
    #Filter the image and a slight blur to improve tracking.
    dilation = cv2.dilate(tracking,kernel,iterations = 1)
    closing = cv2.morphologyEx(dilation, cv2.MORPH_CLOSE, kernel)
    closing = cv2.GaussianBlur(closing,(5,5),0)

    #Combine all HSV windows to 'closing.'
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (7, 7))
    #Close any gaps to complete rectangle and output in new window 'closed.'
    closed = cv2.morphologyEx(closing, cv2.MORPH_CLOSE, kernel)
    #Create a wireframe from 'closed' and output as 'edged.'
    edged = cv2.Canny(closed, 10, 240)

    # Find contours (outlines) in the image and initialize the total number of rectangles found.
    _, cnts, _ = cv2.findContours(closed.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    total = 0

    #Draw the green rectangle and cancel other contours except for the biggest one. 
    if len(cnts) != 0:
        #Draw contours it found in white for refrence.
        cv2.drawContours(frame, cnts, -1, (255, 255, 210), 2)
        #Find the biggest area.
        c = max(cnts, key = cv2.contourArea)
        x,y,w,h = cv2.boundingRect(c)
        #Draw biggest contour.
        cv2.rectangle(frame,(x,y),(x+w,y+h),(0,255,0),2)
###############################################################################

###############################################################################
    ###TRACK THE FIRST RECTANGLE AND OUPUT X AND Y OF EACH CORNER###
###############################################################################
    #Set new threshold
    _, bin = cv2.threshold(closed, 40, 255, 0)

    #Fill holes and make correct image zoom.
    bin = cv2.dilate(bin, None)
    bin = cv2.dilate(bin, None)
    bin = cv2.erode(bin, None)
    bin = cv2.erode(bin, None)

    #Find contours of the second image.
    bin, contours, hierarchy = cv2.findContours(bin, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)

    #Read contours and draw four points of rectangle.
    if len(contours) != 0:
        #Find the biggest area.
        c = max(contours, key = cv2.contourArea)
        rect = cv2.minAreaRect(c)
        box = cv2.boxPoints(rect)

        #Output x and y to NetworkTables and draw circles and numbers.
        namestepper = int(0)
        for p in box:
            namestepper = namestepper + 1
            namex = str('Vision_Point' + str(namestepper) + 'x')
            #x = (int(p[0])-180) #200
	    x = int(p[0])
            namey = str('Vision_Point' + str(namestepper) + 'y')
            #y = (int(p[1])-145) #150
	    y = int(p[1])
            NWTB.putNumber(namex, x)
            NWTB.putNumber(namey, y)
            pt = int(p[0]),int(p[1])
            cv2.circle(frame,pt,5,(255,0,0),2)
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(frame,str(namestepper),pt,font,1,(0,0,255),1,cv2.LINE_AA)

    #Draw crosshairs for calibrating.
    cv2.line(frame,(180,300),(180,0),(0,0,255),1)
    cv2.line(frame,(400,145),(0,145),(0,0,255),1)
                        
    #Show the result in the windows created earlier.
    cv2.imshow('HueComp',hthresh)
    cv2.imshow('SatComp',sthresh)
    cv2.imshow('ValComp',vthresh)
    #cv2.imshow('closing',closing)
    #cv2.imshow("closed", closed)
    cv2.imshow('tracking',frame)
    cv2.imshow('edged',edged)

    #Export the tracking image for driver station view.
    cv2.imwrite('tracking.jpg', tracking)
 
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break
 
cap.release()
f.close()
cv2.destroyAllWindows()
###############################################################################

###################################################################################################







###############################################################################
                ###OLD DETECT AND DRAW CIRCLES###
###############################################################################
#Display the number of rectangles found.
    #print "I found {0} objects in that image".format(total)
 
       #OLD draw circle and write x,y values
 ###   if circles is not None:
 ###           for i in circles[0,:]:
 ###               # If the ball is far, draw it in green
 ###               if int(round(i[2])) < 30:
 ###                   cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,255,0),5)
 ###                   cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,255,0),10)
 ###                   #Create Variables for x and y coordinates
 ###                   x = ((int(round(i[0])))-400)
 ###                   y = ((int(round(i[1])))-300)*-1 
 ###                   #Print x and y values
 ###                   print x,y  
 ###               # else draw it in red
 ###               elif int(round(i[2])) > 35:
 ###                   cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,0,255),5)
 ###                   cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,0,255),10)
 ###                   buzz = 1
 ###                   #Create Variables for x and y coordinates
 ###                   x = ((int(round(i[0])))-800)
 ###                   y = ((int(round(i[1])))-600)*-1
 ###                   #Print x and y values
 ###                   print x,y 
###############################################################################
