#############################################
# Users
# ===========================================
# Purpose is to:
# - What do I do?
#############################################


class User:
    """description of class"""
    def __init__(self, name, rfid, skills, gm, phone):
        if gm:
            self.name = name
            self.rfid = rfid
            self.skills = ["putgrootte","ontkoppelen","aansluiten"]
            self.gm = gm
            self.phone = phone
        else:
            self.name = name
            self.rfid = rfid
            self.skills = skills
            self.gm = False
            self.phone = phone

    
class Users:
    def __init__(self, config_file, parser):
        self.users = []
        self.rfidmap = {}
        usernames = [x.strip() for x in parser.get('common', 'users').split(',')]
        for name in usernames:
            rfid = parser.getint(name,'tag')
            skills = [x.strip() for x in parser.get(name, 'skills').split(',')]
            gm = bool(parser.getint('users', name))
            phone = ""
            if gm:
                phone = parser.get(name, 'phone')
            user = User(name,rfid,skills,gm,phone)
            self.users.append(user)
            self.rfidmap[user.rfid] = user

    def find_user_for_rfid(self, rfid):	
        if rfid in self.rfidmap:
            return self.rfidmap[rfid]
        return None

    # DONE: TODO: change function to set a page to visible if the player (tag) has the correct skill
    def find_pages_for_rfid(self, rfid, menu):
        user = self.find_user_for_rfid(rfid) # Only constructeurs allowed in users list
        if user:
            menu.get_page("main").set_visible(True)
            for skill in user.skills:
                if skill == ("putgrootte"|"ontkoppelen"|"aansluiten"):
                    menu.get_page(skill).set_visible(True)
        else:
            menu.hide_all()
