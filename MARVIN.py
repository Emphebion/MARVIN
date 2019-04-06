import tkinter as tk
from tkinter.ttk import Frame, Button, Style

import Peripheral
import User
import Pages

import configparser
config = configparser.ConfigParser()

SERIAL_BAUD_RATE = 57600

#############################################
# Main function
#============================================
# Purpose is to be a Controller for:
# - Handle button input
# - Update screen
# - Compose and Send message
# - Light Control
#############################################
def main():

    universe = Peripheral.Devices('marvinconfig.txt', config)
    while True:

        x = 1

if __name__ == '__main__':
    main()


