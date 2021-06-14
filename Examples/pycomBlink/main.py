# main.py
import pycom
import time

pycom.heartbeat(False)

red = 0x08
blue = 0x00
green = 0x00
sleepTime = 0.01

def setRgb(red, green, blue):
    rgbValue = 0x000000
    rgbValue |= (red << 16) | (green << 8) | blue
    pycom.rgbled(rgbValue)
    return

while True:
    ###
    #if red >= 0x08:
    #    if green > 0:
    #        green -= 1
    #    else:
    #        blue += 1
    #if blue >= 0x08:
    #    if red > 0:
    #        red -= 1
    #    else:
    #        green += 1
    #if green >= 0x08:
    #    if blue > 0:
    #        blue -= 1
    #    else:
    #        red += 1
    ###
    setRgb(red, green, blue)
    time.sleep(sleepTime)


