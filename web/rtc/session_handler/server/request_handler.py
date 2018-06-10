class RequestHandler:
    def __init__(self, response_generator, room_handler):
        self.response_generator = response_generator
        self.room_handler = room_handler

    def handle_join_room(room_id):
        room_url = self.room_handler.add_or_get(room_id)
        result = self.response_generator.generateRoomInformation(room_url)

    def handle_delete_room(room_id):
        self.room_handler.remove(room_id)

