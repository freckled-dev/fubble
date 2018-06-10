

class RequestParser:
    def parse(self, message):
        types = {
                "joinRoom" : self.parseJoinRoom,
                "deleteRoom" : self.parseDeleteRoom
                }
        message_type = message.WhichOneof("messageType")
        to_call = types[message_type]
        return to_call(getattr(message, message_type))

    def parseJoinRoom(self, message):
        return 'join', message.id

    def parseDeleteRoom(self, message):
        return 'delete', message.id

