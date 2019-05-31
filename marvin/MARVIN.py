import tkinter as tk
from tkinter.ttk import Frame, Button, Style

from marvin import peripheral
from marvin import users
from marvin import pages

import configparser
parser = configparser.ConfigParser()

SERIAL_BAUD_RATE = 57600


#############################################
# Main function
# ===========================================
# Purpose is to be a Controller for:
# - Handle button input
# - Update screen
# - Compose and Send message
# - Light Control
#############################################
def main():

    universe = devices.Devices('marvinconfig.txt', parser)
    menu = pages.Pages('marvinconfig.txt', parser)
    players = users.Users('marvinconfig.txt', parser)
    while True:

        x = 1


if __name__ == '__main__':
    main()
