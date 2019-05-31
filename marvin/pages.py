#############################################
# Pages
# ===========================================
# Purpose is to:
# - Pages is an object that contains all the pages 
#############################################

class Page(object):
    def __init__(self, name):
        self.name = name
        self.visible = False    # page set on visible or not after reading tag

    # Function to display the page
    def toggleVisible(self):
        self.visible != self.visible
        
    


class Pages(object):
    """description of class"""
    def __init__(self, config_file, parser):  # DONE TODO: op de een of andere manier verwijst deze config niet naar het marvinconfig bestand
        parser.read(config_file)
        pagenames = [x.strip() for x in parser.get('pages', name).split(',')]
        self.pages = []
        self.index = 0
        for name in self.pagenames:
            self.pages.append(Page(name))
        
        
        
        
        
        
        
