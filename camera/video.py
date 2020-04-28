import time
import picamera

with picamera.PiCamera() as picam:
    picam.start_preview()
    picam.start_recording('video.h264')
    picam.wait_recording(20)
    picam.stop_recording()
    picam.stop_preview()
    picam.close()
