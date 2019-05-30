#############################################
# Users
# ===========================================
# Purpose is to:
# - What do I do?
#############################################


class User:
    """description of class"""
    def __init__(self, name):
        self.name = name   # TODO: op de een of andere manier verwijst deze config niet naar het marvinconfig bestand
        self.rfid = config.getint('users', name)
        self.skills = [x.strip() for x in config.get('skills', name).split(',')]
        self.set_gamemaster_mode()
        
    def set_gamemaster_mode(self):
        self.GM = 0
        self.GM = config.getint('gamemasters', name)
        if self.GM > 0:
            self.phone = config.getint('phonenumbers', name)
        else:
            self.phone = 0

    
class Users:
    def __init__(self):
        self.load_users()

# TODO: kan dit niet handiger? lijkt 2e keer config te bevragen in user, voor iets dat eerste keer al bekend is
    def load_users(self):
        self.users = []
        self.rfidmap = {}
        usernames = [x.strip() for x in config.get('common', 'users').split(',')]
        for username in usernames:
            user = User(username)
            self.users.append(user)
            self.rfidmap[user.rfid] = user

    def find_user_for_rfid(self, rfid):	
        if rfid in self.rfidmap:
            return self.rfidmap[rfid]
        return None

    def find_pages_for_rfid(self, rfid):
        user = self.find_user_for_rfid(rfid)
        pages = user.build_skill_book()
        return pages
