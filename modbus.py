#!/usr/bin/python


import serial
import time
import sys

# TX: 0e 03 00 00 00 01 84 f5 
# RX: 0e 03 02 00 0e 6d 81 

def to_hex(b):
    s = ""
    for c in b:
        s += "%02x " % c

    return s

ser = serial.Serial()
ser.port = sys.argv[1]
ser.baudrate = sys.argv[2]
ser.timeout = .2
ser.open()

while True:
    if len(sys.argv) > 3 and sys.argv[3] == 'tx':
        a=b"\x0e\x03\x00\x00\x00\x01\x84\xf5"
        print(f"TX: {to_hex(a)}")
        ser.write(a)
    
    while True:
        rx = ser.read(10)
        if len(rx) == 0:
            break

        print(f"RX: {to_hex(rx)}")    

#for i in range(50):
#    line = ser.readline()
#    print(line)

