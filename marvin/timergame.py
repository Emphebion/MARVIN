#############################################
# Timer game
# ===========================================
# Purpose is to:
# - keep player busy
#############################################

import random
import time
import buttons
import screen

class Timergame(object):

    def __init__(self):
        self.buttons = {buttons.Button(1), buttons.Button(2), buttons.Button(3), buttons.Button(4)}

    def play(self, minutes):
        t_end = time.time() + 60 * minutes  # now + 60 seconds * ? minutes
        while time.time() < t_end:
            self.random_button_quest(random.randint(1, 5))
            self.pause(random.randint(5, 50))

    def random_button_quest(self, times):
        i = 0
        while i < times:
            screen.ask_for_button_push(buttons[random.randint(0, 3)])  # enum ipv hele button meegeven?
            if button(button_x) = pushed
                i += 1
            else
                fail()

    def fail(self):
        screen.fail_screen()  # display failscreen for ? seconds
        # go to menu start page

    @staticmethod
    def pause(seconds):  # TODO: kan dit met time.sleep?
        t_end = time.time() + seconds
        while time.time() < t_end:
            # do nothing, just wait
            # of een tellertje op het scherm dat aftelt tot de volgende actie?
            # TODO: of een dansend gifje? }:-)
            pass
