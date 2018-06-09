from server.request_parser import RequestParser
from server.fubble_rtc_pb2 import JoinRoom, DeleteRoom, RtcMessageRequest

def test_construction():
    class Callback:
        pass
    callback = Callback()
    request_parser = RequestParser(callback)
    assert request_parser.callback == callback

class CallbackJoinedRoom:
    def handle_join_room(self, room_id):
        assert "room_id" not in self.__dict__
        self.room_id = room_id
        assert "room_id" in self.__dict__

def test_parse_join_room():
    callback = CallbackJoinedRoom()
    request_parser = RequestParser(callback)
    joinRoom = JoinRoom()
    joinRoom.id = "uuid"
    request_parser.parseJoinRoom(joinRoom)
    assert callback.room_id == "uuid"

def test_parse_with_join_room():
    callback = CallbackJoinedRoom()
    request_parser = RequestParser(callback)
    request = RtcMessageRequest()
    request.joinRoom.id = "uuid"
    request_parser.parse(request)
    assert callback.room_id == "uuid"


class CallbackDeleteRoom:
    def handle_delete_room(self, room_id):
        assert "room_id" not in self.__dict__
        self.room_id = room_id
        assert "room_id" in self.__dict__

def test_parse_delete_room():
    callback = CallbackDeleteRoom()
    request_parser = RequestParser(callback)
    deleteRoom = DeleteRoom()
    deleteRoom.id = "uuid"
    request_parser.parseDeleteRoom(deleteRoom)
    assert callback.room_id == "uuid"

def test_parse_with_delete_room():
    callback = CallbackDeleteRoom()
    request_parser = RequestParser(callback)
    request = RtcMessageRequest()
    request.deleteRoom.id = "uuid"
    request_parser.parse(request)
    assert callback.room_id == "uuid"


