from PIL import Image, ImageTk

class Page:
    def __init__(self):
        self.name = name #copy from image name
        self.image = 0

    # Function to display the page
    def display(self):
        pass

class Pages:
    """description of class"""
    def __init__(self):
        pagelist = [x.strip() for x in config.get('pages', name).split(',')]
        self.pages = ['home']
        self.index = 0

class Menu:
    def __init__(self):
        self.menu = []
