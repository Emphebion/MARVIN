#############################################
# Devices
# ===========================================
# Purpose is to:
# - Who am I?
#############################################

# -*- coding: utf-8 -*-
# GNU General Public License v3

import serial
import time
import re
import subprocess

SERIAL_BAUD_RATE = 57600


class Devices(object):  # TODO: is dit dezelfde als in peripheral?
    def __init__(self, config_file, parser):
        self.parse_config(config_file, parser)
        self.devices = self.connected_serial_devices()

    def parse_config(self, config_file, parser):
        parser.read(config_file)

        self.devIDs = parser.get('devices', 'IDs').split(',')

    def connected_serial_devices(self):
        device_re = re.compile(b'Bus\s+(?P<bus>\d+)\s+Device\s+(?P<device>\d+).+ID\s(?P<id>\w+:\w+)\s(?P<tag>.+)', re.I)
        df = subprocess.check_output("lsusb")
        devices = []
        for i in df.split(b'\n'):
            if i:
                info = device_re.match(i)
                if info:
                    dinfo = info.groupdict()
                    dinfo['device'] = '/dev/bus/%s/%s' % (dinfo.pop('bus'),dinfo.pop('device'))
                    devices.append(dinfo)
        for device in devices:
            print(device)
        return devices


class Device:

    def __init__(self, name, port):
        self.name = name
        self.port = port
        self.connect(self.port)

    def connect(self, port, timeout=0.1):
        self.arduino = serial.Serial(port, SERIAL_BAUD_RATE, timeout=timeout)
        time.sleep(2)

    def get_device(self, name):
        return self.name