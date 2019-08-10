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
    def toggle_visible(self):
        self.visible != self.visible
        
    def set_visible(self, value):
        self.visible = value


class Pages(object):
    """description of class"""
    def __init__(self, config_file, parser): 
        parser.read(config_file)
        self.pagenames = map(lambda x:x.strip(), parser.get('common', 'pages').split(','))
        self.index = 0
        self.pages = []
        for name in self.pagenames:
            self.pages.append(Page(name))

    def get_page(self,name):
        for page in self.pages:
            if page.name = name:
                return page

    def sel_next_page(self):
        ind = self.index
        self.index = self.index + 1
        while ind != self.index:
            if(self.index >= len(self.pages)):
                self.index = 1
            if(self.pages[self.index].visible):
                return self.pages[self.index].name
            self.index = self.index + 1
        return self.pages[ind].name

    def sel_prev_page(self):
        ind = self.index
        self.index = self.index - 1
        while ind != self.index:
            if(self.index <= 0):
                self.index = len(self.pages) - 1
            if(self.pages[self.index].visible):
                return self.pages[self.index].name
            self.index = self.index - 1
        return self.pages[ind].name

    """Returns the name of the current page to be used to determine the correct skill"""
    def sel_option(self):
        return self.pages[self.index].name

    def hide_all(self):
        for page in self.pages:
            page.set_visible(False)