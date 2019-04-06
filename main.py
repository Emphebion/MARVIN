# -*- coding: utf-8 -*-
# GNU General Public License v3

# This is a sample yet useful program, for python 3

import devices
import configparser
config = configparser.ConfigParser()

def main():
    dev_array = devices.Devices('config.txt', config)
    print(str(dev_array.devIDs))

if __name__ == '__main__':
    main()
