"""Control arduino board with 1-wire sensors and relays connected"""

import serial

serialport = serial.Serial("/dev/ttyUSB0", 9600, timeout=0.5)

while True:    
    command = serialport.readline()
    print(command)