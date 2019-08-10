#############################################
# Items
# ===========================================
# Purpose is to:
# - What am I doing here?
#############################################


class Item(object):
    def __init__(self, name, load=1, connected=False):  # TODO: dit moet nog ergens opgehaald worden....
        self.name = name
        self.load = load
        self.connected = connected

    def toggle_connected(self):
        self.connected != self.connected
        
    def set_connected(self, value):
        self.connected = value

class Items(object):
    def __init__(self, config_file, parser):
        self.items = []
        self.index = 0
        self.itemnames = [x.strip() for x in parser.get('common', 'items').split(',')]
        for name in self.itemnames:
            load = [x.strip() for x in parser.get(name, 'load').split(',')]
            connected = bool([x.strip() for x in parser.get(name, 'connected').split(',')])
            self.items.append(Item(name,load,connected))

    def sel_next_item(self):
        ind = self.index
        self.index = self.index + 1
        while ind != self.index:
            if(self.index >= len(self.items)):
                self.index = 0
            if(self.items[self.index].visible):
                return self.items[self.index].name
            self.index = self.index + 1
        return self.items[ind].name

    def sel_prev_item(self):
        ind = self.index
        self.index = self.index - 1
        while ind != self.index:
            if(self.index <= 0):
                self.index = len(self.items) - 1
            if(self.items[self.index].visible):
                return self.items[self.index].name
            self.index = self.index - 1
        return self.items[ind].name

    def sel_curr_item(self):
        self.items[self.index].set_connected(True)

    def calc_put(self):
        put = 0
        for item in self.items:
            if item.connected:
                put = put + item.load
        return put

    def disconnect_all(self):
        for item in self.items:
            item.set_connected(False)
