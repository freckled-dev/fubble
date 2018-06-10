import server.web_rtc_pb2

def test_construction_server(rtc_server):
    pass

def test_construction_stub(rtc_stub):
    assert "JoinRoom" in rtc_stub.__dict__

def test_join_room(rtc_stub):
    response = rtc_stub.JoinRoom(server.web_rtc_pb2.JoinRoomRequest(id="room id"))
    assert response.rtc_server == "https://rtc.fubble.io/room id"

def test_delete_room(rtc_stub):
    response = rtc_stub.JoinRoom(server.web_rtc_pb2.JoinRoomRequest(id="room id"))
    assert response.rtc_server == "https://rtc.fubble.io/room id"
    no_response = rtc_stub.DeleteRoom(server.web_rtc_pb2.DeleteRoomRequest(id="room id"))

