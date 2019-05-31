#############################################
# Screen
# ===========================================
# Purpose is to:
# - What am I doing here?
#############################################

import pygame

class Screen:
    def __init__(self, config_file, parser):
        parser.read(config_file)
        self.size = [x.strip() for x in parser.get('size', name).split('x')]
        pygame.init()
        screen = pygame.display.set_mode(size[1],size[2])
        
    def display(self, name, location)
        image = pygame.image.load(name)
        screen.blit(image,[location[0],location[1]])
        pygame.display.flip()