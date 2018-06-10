from server import fubble_rtc_pb2

def test_join_room():
    joinRoom = fubble_rtc_pb2.JoinRoom()
    joinRoom.id = 'uuid'
    serializied = joinRoom.SerializeToString()

    parsedJoinRoom = fubble_rtc_pb2.JoinRoom()
    parsedJoinRoom.ParseFromString(serializied)
    assert parsedJoinRoom.id == joinRoom.id

def test_rtc_message_request_0():
    request = fubble_rtc_pb2.RtcMessageRequest()
    assert request.WhichOneof("messageType") is None
    request.joinRoom.id = 'uuid'
    assert request.WhichOneof("messageType") == "joinRoom"
    assert request.HasField("joinRoom")
    assert not request.HasField("deleteRoom")

def test_rtc_message_request_1():
    joinRoom = fubble_rtc_pb2.JoinRoom()
    joinRoom.id = 'uuid'
    request = fubble_rtc_pb2.RtcMessageRequest()
    assert request.WhichOneof("messageType") is None
    request.joinRoom.CopyFrom(joinRoom)
    assert request.WhichOneof("messageType") == "joinRoom"
    assert request.HasField("joinRoom")
    assert not request.HasField("deleteRoom")

