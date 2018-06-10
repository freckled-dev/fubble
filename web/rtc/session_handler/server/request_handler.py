class RequestHandler:
    def __init__(self, response_generator, room_handler):
        self.response_generator = response_generator
        self.room_handler = room_handler

    def handle(self, type_, data):
        types = {
                "join": self.handle_join_room,
                "delete": self.handle_delete_room
                }
        method = types[type_]
        return method(data)

    def handle_join_room(self, message):
        room_id = message.id
        room_url = self.room_handler.add_or_get(room_id)
        result = self.response_generator.generateRoomInformation(room_url)
        return result

    def handle_delete_room(self, room_id):
        self.room_handler.remove(room_id)

