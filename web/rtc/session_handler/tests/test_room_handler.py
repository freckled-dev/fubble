from server.room_handler import RoomHandler
from server.room_url_generator import RoomUrlGenerator

def create_room_handler():
    url_generator = RoomUrlGenerator("prefix_")
    result = RoomHandler(url_generator)
    return result

def test_construction():
    rooms = create_room_handler()
    assert not rooms.rooms

def test_add_or_get():
    rooms = create_room_handler()
    added = rooms.add_or_get("room name")
    assert added == "prefix_room name"
    assert len(rooms.rooms) == 1

def test_get_fail():
    rooms = create_room_handler()
    try:
        rooms.get("room name")
    except RoomHandler.NotFoundError:
        return
    assert false

def test_get():
    rooms = create_room_handler()
    rooms.add_or_get("room name")
    got = rooms.get("room name")
    assert got == "prefix_room name"

def test_remove():
    rooms = create_room_handler()
    rooms.add_or_get("room name")
    assert len(rooms.rooms) == 1
    got = rooms.remove("room name")
    assert not rooms.rooms

def test_remove_fail():
    rooms = create_room_handler()
    try:
        rooms.remove("room name")
    except RoomHandler.NotFoundError:
        return
    assert false
