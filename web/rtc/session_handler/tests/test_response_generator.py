from server.response_generator import ResponseGenerator

def test_construction():
    to_test = ResponseGenerator()

def test_generate_room_information():
    to_test = ResponseGenerator()
    response = to_test.generateRoomInformation("url")
    assert response.WhichOneof("messageType") == "roomInformation"
    assert response.roomInformation.rtcServer == "url"

