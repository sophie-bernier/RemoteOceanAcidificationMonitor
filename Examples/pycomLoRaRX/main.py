# main.py
# Example from pycom docs:
# https://docs.pycom.io/tutorials/networks/lora/lora-mac/

from network import LoRa
import pycom
import socket
import machine
import time

# initialise LoRa in LORA mode
# Please pick the region that matches where you are using the device:
# Asia = LoRa.AS923
# Australia = LoRa.AU915
# Europe = LoRa.EU868
# United States = LoRa.US915
# more params can also be given, like frequency, tx power and spreading factor
lora = LoRa(mode=LoRa.LORA, region=LoRa.US915)
pycom.heartbeat(False)

# create a raw LoRa socket
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)

rgbVal = 0x000000

while True:
    # # send some data
    # s.setblocking(True)
    # s.send('Hello')

    # get any data received...
    s.setblocking(False)
    data = s.recv(64)
    print(data)

    if rgbVal == 0:
        rgbVal = 0x001100
    else:
        rgbVal = 0
    
    pycom.rgbled(rgbVal)

    # wait a random amount of time
    # time.sleep(machine.rng() & 0x0F)

    # wait a fixed amount of time
    time.sleep(1)