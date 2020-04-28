import time
import picamera
import os
path=os.getenv("HOME")+""  #adjust path for location of this program
with picamera.PiCamera() as picam:
    picam.rotation=0 #angle setting
    picam.start_preview()
#    int i = 1
#    while(true) :
#        time.sleep(1) # delay time
#        picam.capture('pic'+i+'.jpg')
    time.sleep(3) # delay time
    picam.capture('./pic1.jpg')
#        i++
    # picam.capture(path+'/photo.jpg',resize=(640,480))
    # time.sleep(2)
    picam.stop_preview()
    picam.close()
