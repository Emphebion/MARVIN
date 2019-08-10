import tkinter as tk
from tkinter.ttk import Frame, Button, Style

from devices import Devices
from users import Users
from pages import Pages
from items import Items
from screen import Screen

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

    display = Screen('marvinconfig.txt',parser)
    menu = Pages('marvinconfig.txt', parser)
    artifacts = Items('marvinconfig.txt', parser)
    players = Users('marvinconfig.txt', parser)
    universe = Devices('marvinconfig.txt', parser)
    while True:

        '''TODO:
            - Check if a correct tag is presented
            - set pages to visible based on tag
            - show opening page
            - wait on button and then:
                - load new page/execute skill
                - set main menu page as invisible (once)
            - on execute skill:
                - start minigame
            - set pages to invisible'''


if __name__ == '__main__':
    main()
