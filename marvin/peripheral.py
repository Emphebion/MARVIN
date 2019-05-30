import re
from re import split

import serial
import time
import threading
import subprocess


#############################################
# Peripheral function
# ===========================================
# Purpose is to:
# - Setup connections to the periferals
#############################################
class Periph(object):
    def __init__(self, name, port):
        self.name = name
        self.port = port
        self.connect(self.port)

    def connect(self, port, timeout=0.1):  # TODO: ik zie een serial_baud_rate in MARVIN en in devices
        self.usbcom = serial.Serial(port, SERIAL_BAUD_RATE, timeout=timeout)
        self.thread = threading.Thread(target=read)
        self.thread.start()
        time.sleep(2)

    def send(self, command):
        crc = (ord(command) ^ self.data[0] ^ self.data[1] ^ self.data[2])
        n = self.usbcom.write(self.format_msg(command, self.data[0], self.data[1], self.data[2], crc))

    def format_msg(self, address, command, data0, data1, data2, data3):
        return b''.join([START_BYTE, chr(address), command, chr(data0), chr(data1), chr(data2), END_BYTE, chr(data3)])

    def read(self):
        time.sleep(.000001)
        if self.usbcom.readable():
            data = self.usbcom.readline()
            if not data:
                return None
            else:
                retValue = self.parse_status_response(data)
                return retValue

    def parse_status_response(data):
        i = 0
        crc = 0
        tag = 0
        startFound = False
        endFound = False
        value = 0

        while i < 8:
            dataByte = data[i]

            if dataByte == START_BYTE:
                startIndex = i
                startFound = True

            elif ((dataByte == END_BYTE) and (endFound == False)):
                endFound = True

            else:
                if (startFound == True) and (endFound == False):
                    crc ^= ord(dataByte)
                    if i >= (startIndex + 2):
                        value = value << 8
                        value ^= ord(dataByte)

                if endFound == True:
                    checksum = ord(dataByte)
                    if checksum == crc:
                        return value
                    else:
                        return 0

            i = i + 1
        return 0


#############################################
# Interconnect function
# ===========================================
# Purpose is to:
# - Setup connections to the periferals
#############################################
class Devices(object):  # TODO: is dit dezelfde als in devices?
    def __init__(self, config_file, parser):
        self.parse_config(config_file, parser)

    def parse_config(self, config_file, parser):
        parser.read(config_file)

        self.devicenames = map(lambda x:x.strip(), parser.get('common', 'devices').split(','))
        self.deviceIDs = map(lambda x:x.strip(), parser.get('common', 'devIDs').split(','))
        self.namesIDsDict = dict(zip(self.deviceIDs,self.devicenames))
        self.portmap = self.port_selection_per_id(self.namesIDsDict)
        self.connectedDevices = []
        for current in self.devicenames:
            self.connectedDevices.append(Periph(current, self.portmap(current)))

    def port_selection_per_id(self, iddict):
        ttyUSBList = subprocess.check_output("ls /dev/ttyU*")
        portSort = []
        nameSort = []
        for port in ttyUSBList:
            tempID = split(subprocess.check_output("grep PRODUCT= /sys/bus/usb-serial/devices/%s/../uevent", port), '/')
            productID = tempID[0] + ":" + tempID[1]
            print(productID)
            portSort.append(port)
            nameSort.append(iddict(productID))
        '''return namePortDict = dict(zip(nameSort,portSort))'''

    def connected_serial_devices(self):
        device_re = re.compile(b'Bus\s+(?P<bus>\d+)\s+Device\s+(?P<device>\d+).+ID\s(?P<id>\w+:\w+)\s(?P<tag>.+)', re.I)
        df = subprocess.check_output("lsusb")
        foundDevices = []
        for i in df.split(b'\n'):
            if i:
                info = device_re.match(i)
                if info:
                    dinfo = info.groupdict()
                    dinfo['device'] = '/dev/bus/%s/%s' % (dinfo.pop('bus'),dinfo.pop('device'))
                    foundDevices.append(dinfo)
        for device in foundDevices:
            print(device)
        return foundDevices

    def get_device(self, name):
        return self.connDevices[name]
