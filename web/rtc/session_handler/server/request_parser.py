

class RequestParser:
    def __init__(self, callback):
        self.callback = callback

    def parse(self, message):
        types = {
                "joinRoom" : self.parseJoinRoom,
                "deleteRoom" : self.parseDeleteRoom
                }
        message_type = message.WhichOneof("messageType")
        to_call = types[message_type]
        to_call(getattr(message, message_type))

    def parseJoinRoom(self, message):
        self.callback.handle_join_room(message.id)

    def parseDeleteRoom(self, message):
        self.callback.handle_delete_room(message.id)

