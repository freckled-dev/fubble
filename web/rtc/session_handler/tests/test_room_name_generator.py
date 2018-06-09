from server.room_url_generator import RoomUrlGenerator

def test_construction():
    test = RoomUrlGenerator("prefix")
    assert test.url_base == "prefix"

def test_generate():
    generator = RoomUrlGenerator("prefix_")
    generated = generator.generate("name")
    assert generated == "prefix_name"
