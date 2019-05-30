#############################################
# Serialtest
# ===========================================
# Purpose is to:
# - What is my purpose?
#############################################

import serial as serial


class SerialTest:
    
    def __init__(self):
        self.port = 'dev/ttyUSB0'
        self.baud = 9600
        
    def test(self):
        ser = serial.Serial(self.port, self.baud)
            s = [0, 1]
            while True:
                #read_serial = ser.readline()
                s[0] = str(int(ser.readline(), 16))
                print(s[0])
                #print(read_serial)