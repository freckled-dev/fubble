import server.web_rtc_pb2

def test_construction_server(rtc_server):
    pass

def test_construction_stub(rtc_stub):
    assert "JoinRoom" in rtc_stub.__dict__

def test_join_room(rtc_stub):
    response = rtc_stub.JoinRoom(server.web_rtc_pb2.JoinRoomRequest(id="room id"))

