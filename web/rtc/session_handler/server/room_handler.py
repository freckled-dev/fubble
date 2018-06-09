class RoomHandler:
    class NotFoundError(Exception):
        pass

    def __init__(self, url_generator):
        self.url_generator = url_generator
        self.rooms = {}

    def add_or_get(self, name):
        if name in self.rooms:
            return self.get(name)
        room_url = self.url_generator.generate(name)
        self.rooms[name] = room_url
        return room_url

    def get(self, name):
        if not name in self.rooms:
            raise RoomHandler.NotFoundError()
        return self.rooms[name]

    def remove(self, name):
        if not name in self.rooms:
            raise RoomHandler.NotFoundError()
        del self.rooms[name]

