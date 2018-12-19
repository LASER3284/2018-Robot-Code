import argparse
import cv2
import time
from VideoProcess import VideoProcess
from VideoGet import VideoGet
from VisionNetworking import VisionNetworking

def putIterationsPerSec(frame, iterations_per_sec):
    '''
    Add iterations per second text to lower-left corner of a frame.
    '''
    cv2.putText(frame, "{:.0f} iterations/sec".format(iterations_per_sec),
        (10, 450), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (255, 255, 255))
    return frame

def noThreading(source=0):
    '''
    Grab and show video frames without multithreading.
    '''
    cap = cv2.VideoCapture(source)
    cps = CountsPerSec().start()

    while True:
        (grabbed, frame) = cap.read()
        if not grabbed or cv2.waitKey(1) == ord("q"):
            break

        frame = putIterationsPerSec(frame, cps.countsPerSec())
        cv2.imshow("Video", frame)
        cps.increment()
        
def threadBoth(gui, server, source=0):
    '''
    Dedicated thread for grabbing video frames with VideoGet object.
    Dedicated thread for processing and optionally showing video frames
    with VideoShow object. Main thread serves only to pass frames between
    VideoGet and VideoProcess objects/threads.
    '''

    # Create Object Pointers
    video_getter = VideoGet(source).start()
    video_processor = VideoProcess(gui, video_getter.frame, video_getter.resolution).start()
    vision_networking = VisionNetworking(gui, server, video_processor.pointArray, video_getter.resolution).start()

    # Main multithreading loop.
    while True:
        if video_getter.stopped or video_processor.stopped:
            video_processor.stop()
            vision_networking.stop()
            video_getter.stop()
            break

        # Pass camera frame between classes.
        frame = video_getter.frame                        # Grab frame from camera.
        video_processor.frame = frame                     # Push camera frame to VideoProcess class for vision processing.
        resolution = video_getter.resolution              # Grab camera resolution.
        vision_networking.resolution = resolution         # Push camera resolution to VideoNetworking class and VideoProcess class.
        video_processor.resolution = resolution

        # Pass required info between classes.
        trackbarValues = vision_networking.trackbarValues # Pass trackbar values from VisionNetworking to VisionProcess.
        video_processor.trackbarValues = trackbarValues
        pointArray = video_processor.pointArray           # Pass vision points from VideoProcess to VisionNetworking.
        vision_networking.pointArray = pointArray
        mode = vision_networking.mode                     # Pass vision mode from VisionNetworking to VideoProcess
        video_processor.mode = mode
        speed = video_processor.speed                     # Pass vision speed from VideProcess to VisionNetworking
        vision_networking.speed = speed
            
        # Add delay so other threads can catch up.
        time.sleep(0.02)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--source", "-s", default=0,
        help="Path to video file or integer representing webcam index"
            + " (default 0).")
    ap.add_argument("--thread", "-t", default="none",
        help="Threading mode: both (video read and video show in their own threads),"
            + " none (default--no multithreading)")
    ap.add_argument("--gui", "-g", default="no",
        help="Graphical User Interface: yes (show video windows for vision tuning),"
            + " no (default, run supa fast)")
    ap.add_argument("--server", "-S", default="10.32.84.2",
        help="Server IP for NetworkTables communication. (default: 10.32.84.2)")
    args = vars(ap.parse_args())

    if args["thread"] == "both":
        threadBoth(args["gui"], args["server"], args["source"])
    else:
        noThreading(args["source"])

if __name__ == "__main__":
    main()