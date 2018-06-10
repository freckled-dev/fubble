from server import create_app
from server.fubble_rtc_pb2 import JoinRoom, RtcMessageRequest

def test_config():
    assert not create_app().testing
    assert create_app({'TESTING': True}).testing

def test_join_room(client):
    response = client.get('/')
    assert response.data == b'hello world'

# def test_join_room(client):
    # message = RtcMessageRequest()
    # message.joinRoom.id = "uuid"
    # messageSerialized = message.SerializeToString()
    # response = client.get('/', data = messageSerialized)

