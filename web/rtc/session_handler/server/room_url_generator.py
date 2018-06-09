class RoomUrlGenerator:
    def __init__(self, url_base):
        self.url_base = url_base

    def generate(self, room_name):
        return self.url_base + room_name

