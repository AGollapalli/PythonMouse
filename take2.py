import struct
# import math
from pymouse import PyMouse
import bluepy.btle
import Xlib
from time import gmtime, strftime

dt = 0.01
k = 50
mouse = PyMouse()
xscan = bluepy.btle.Scanner(0)
dx = 0
dy = 0
dz = 0
connected = 0

print(strftime("%Y-%m-%d %H:%M:%S", gmtime()))

xscan.scan()
deviceList = xscan.getDevices()
print(deviceList)
for i in deviceList:
    print(i.getScanData())
    for j in i.getScanData():
        print(j[-1])
        if j[-1] == 'stylus':
            stylus = bluepy.btle.Peripheral(i.addr)
            print("EULALIA!! Stylus is connected!")
            connected = 1
            break
if stylus:
    # delegate = stylus.DefaultDelegate()
    print(stylus.getCharacteristics())
    for i in stylus.getCharacteristics():
        print(i.uuid)
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

        xint = struct.unpack('s', xchar.read())[0]
        yint = struct.unpack('s', ychar.read())[0]
        zint = struct.unpack('s', zchar.read())[0]

        # d2x = math.floor(k * xint) * dt
        # d2y = math.floor(k * yint) * dt
        # d2z = math.floor(k * (zint + 16500)) * dt

        # dx = ((dx / dt) + d2x) * dt

        # dy = ((dy / dt) + d2y) * dt
        # dz = ((dz / dt) + d2z) * dt

        position = mouse.position()

        newx = position[0] + xint
        newy = position[1] + zint

        print(xint)
        print("\t")
        print(zint)
        print("\n")
        mouse.move(newx, newy)
