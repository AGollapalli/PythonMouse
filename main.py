import struct
import math
from pymouse import PyMouse
import bluepy.btle
import Xlib

dt = 0.01
k = 50
mouse = PyMouse()
xscan = bluepy.btle.Scanner(0)
dx = 0
dy = 0
dz = 0
connected = 0

xscan.scan()
deviceList = xscan.getDevices()
print(deviceList)
for i in deviceList:
    print(i.getScanData())
    for j in i.getScanData():
        print j[-1]
        if j[-1] == 'stylus':
            stylus = bluepy.btle.Peripheral(i.addr)
            print("EULALIA!! Stylus is connected!")
            connected = 1
            break
if stylus:
 #   delegate = stylus.DefaultDelegate()
    print stylus.getCharacteristics()
    for i in stylus.getCharacteristics():
        print i.uuid
        if i.uuid == "AB126969-AB12-6969-AB12-AB126969AB13":
            xchar = i
        elif i.uuid == "AB126969-AB12-6969-AB12-AB126969AB14":
            ychar = i
        elif i.uuid == "AB126969-AB12-6969-AB12-AB126969AB15":
            zchar = i
if xchar and ychar and zchar:
    while connected == 1:
         # for i in range(1 , 1000, 1):
         #     print (
         #         struct.unpack('i', xchar.read())[0] + "\t"
         #         + struct.unpack('i', ychar.read())[0] + "\t"
         #         + struct.unpack('i', zchar.read())[0])

        xint = struct.unpack('i', xchar.read())[0]
        yint = struct.unpack('i', ychar.read())[0]
        zint = struct.unpack('i', zchar.read())[0]

        d2x = math.floor(k * xint) * dt
        d2y = math.floor(k * yint) * dt
        d2z = math.floor(k * (zint + 16500)) * dt

        dx = ((dx / dt) + d2x) * dt

        dy = ((dy / dt) + d2y) * dt
        dz = ((dz / dt) + d2z) * dt

        position = mouse.position()

        mouse.move(position[0] + dx, position[1] + dz)
