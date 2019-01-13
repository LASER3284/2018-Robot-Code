from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from SocketServer import ThreadingMixIn
from threading import Thread
import threading
from PIL import Image
import StringIO
import numpy
import time
import cv2

class socketHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.endswith(".mjpg"):
            self.send_response(200)
            self.send_header("Content-type", "multipart/x-mixed-replace; boundary=--jpgboundary")
            self.end_headers()
            while True:
                try:
                    img = streamFrame
                    scalePercent = 270
                    width = int(img.shape[1] * scalePercent / 100)
                    height = int(img.shape[0] * scalePercent / 100)
                    dim = (width, height)
                    # Resize image
                    resized = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
                    imgRGB = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
                    jpg = Image.fromarray(imgRGB)
                    tmpFile = StringIO.StringIO()
                    jpg.save(tmpFile, "JPEG")
                    self.wfile.write("--jpgboundary")
                    self.send_header("Content-type", "image/jpeg")
                    self.send_header("Content-length", str(tmpFile.len))
                    self.end_headers()
                    jpg.save(self.wfile, "JPEG")
                    time.sleep(0.05)
                except stopped:
                    break
            return
        if self.path.endswith(".html"):
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write("<html><head></head><body>")
            self.wfile.write('<img src="http://192.168.43.106:8080/cam.mjpg"/>')
            self.wfile.write("</body></html>")
            return

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    '''
    Handle requests in a separate thread.
    ''' 

class HTTPRefresher():
    '''
    Constantly searches for new requests.
    '''
    def start(self):
        Thread(target=self.refresh, args=()).start()
        return self
    
    def refresh(self):
        # Start HTTP server and process requests.
        server = ThreadedHTTPServer(("192.168.43.106", 8080), socketHandler)
        print "MJPG Streaming Server Started..."
        server.serve_forever()

class VideoStream():
    def __init__(self, stopped, processedFrame = None):
        # Create local variables.
        self.processedFrame = processedFrame
        self.stopped = False

    def start(self):
        Thread(target=self.stream, args=()).start()
        return self

    def stream(self):
        # Create global variables.
        global streamFrame
        global stopped
        global img
        stopped = False
        
        # Run refresher in different class so that VideoStream can pass processedFrame.
        HTTPRefresher().start()
        
        # Pass processedFrame to socketHandler.
        while not self.stopped:
            streamFrame = self.processedFrame
            stopped = self.stopped
            time.sleep(0.1)

    def stop(self):
        self.stopped = True