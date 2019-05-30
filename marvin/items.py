#############################################
# Items
# ===========================================
# Purpose is to:
# - What am I doing here?
#############################################


class Items:
    def __init__(self):
        self.load_items()

    def load_items(self):
        self.items = []
        self.item_dict = {}
        itemnames = [x.strip() for x in config.get('items', 'names').split(',')]
        for itemname in itemnames:
            item = Item(itemname)
            self.items.append(item)
            self.item_dict[item.name] = item

class Item:
    def __init__(self, name, size=1, powered=False):  # TODO: dit moet nog ergens opgehaald worden....
        self.name = name
        self.size = size
        self.powered = powered