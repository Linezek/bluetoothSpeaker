from scipy.io.wavfile import read
filename = "test7.wav"
rate, data2 = read(filename)

data = [x for x in data2]

import numpy as np
max = np.max(data2)
min = np.min(data2)

# extract sound each 0.1 seconde
time = len(data) / rate # en seconde
timeSplit = 1 / 0.1
time *= timeSplit

incrementRate = len(data) / time
i = 0
res = []
while i < time:
    value = data[ int(i * incrementRate) ]
    #print(value)
    #value += 32768
    #value /= 65535
    res.append(float(value))
    i += 1

print("Original time : ", len(data)/rate)
print("Secondes between two value ", (len(data) / rate) / time)
print("Nb of notes", len(res))

#c8:b2:7b:87:7b:eb
import logging
import asyncio

from bleak import BleakClient
import time
import sys


def police():
    res = []
    aw = []
    i = 0
    while i < 20:
        res.append(969.0/8000.0*255.0)
        aw.append(0.5)
        res.append(800.0/8000.0*255.0)
        aw.append(0.5)
        i = i + 2
    res.append(0)
    aw.append(1)
    return res, aw


def happy_birthday():
    A4 = 440        
    B4b = 446         
    C4 = 261
    C4_1 = 130 
    C5 = 523      
    D4 = 293   
    D4b = 277               
    E4 = 329          
    F4 = 349           
    G4 = 392  
    notes = [C4_1,C4,D4,C4,F4,E4,C4_1,C4,D4,C4,G4,F4,C4_1,C4,C5,A4,F4,E4,D4,B4b,B4b,A4,F4,G4,F4]
    intervals = [4, 4, 8, 8, 8, 10, 4, 4, 8, 8, 8, 10, 4, 4, 8, 8, 8, 8, 8, 4, 4, 8, 8, 8, 12]   
    res = []
    aw = []
    for i in range(24):
        res.append(notes[i]/8000.0*255.0)
        aw.append(0.8*float(intervals[i])/10.0)
    res.append(0)
    aw.append(1)
    return res, aw

import random

async def run(address, loop, debug=False):
    log = logging.getLogger(__name__)
    if debug:
        # loop.set_debug(True)
        log.setLevel(logging.DEBUG)
        h = logging.StreamHandler(sys.stdout)
        h.setLevel(logging.DEBUG)
        log.addHandler(h)

    # try until you can connect
    while (1):
        try:
            async with BleakClient(address, loop=loop) as client:
                x = await client.is_connected()
                log.info("Connected: {0}".format(x))
                for service in client.services:
                    for char in service.characteristics:
                        # player Music
                        if char.properties[0] == 'write':
                            v = random.randint(0, 2)
                            if v == 1:
                                res, aw = police()
                            else:
                                res, aw = happy_birthday()                                

                            for ct in range(len(res)):
                                i = int(res[ct])# * 255)
                                print("Send:", bytes([i]))
                                value = await client.write_gatt_char(char.uuid, bytes([i]))
                                time.sleep(aw[ct])
                print("DataSend")
        
        except Exception as e:
            print(e)
            continue

if __name__ == "__main__":
    # device address
    address = "c8:b2:7b:87:7b:eb"
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run(address, loop, True))