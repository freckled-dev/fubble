from server.request_parser import RequestParser
from server.fubble_rtc_pb2 import JoinRoom, DeleteRoom, RtcMessageRequest

def test_construction():
    request_parser = RequestParser()

def test_parse_join_room():
    request_parser = RequestParser()
    joinRoom = JoinRoom()
    joinRoom.id = "uuid"
    type_, data = request_parser.parseJoinRoom(joinRoom)
    assert data == "uuid"
    assert type_ == "join"

def test_parse_with_join_room():
    request_parser = RequestParser()
    request = RtcMessageRequest()
    request.joinRoom.id = "uuid"
    type_, data = request_parser.parse(request)
    assert data == "uuid"
    assert type_ == "join"

def test_parse_delete_room():
    request_parser = RequestParser()
    deleteRoom = DeleteRoom()
    deleteRoom.id = "uuid"
    type_, data = request_parser.parseDeleteRoom(deleteRoom)
    assert data == "uuid"
    assert type_ == "delete"

def test_parse_with_delete_room():
    request_parser = RequestParser()
    request = RtcMessageRequest()
    request.deleteRoom.id = "uuid"
    type_, data = request_parser.parse(request)
    assert data == "uuid"
    assert type_ == "delete"


