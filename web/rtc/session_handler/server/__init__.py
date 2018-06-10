import os

# from flask import Flask, request
# from server.room_handler import RoomHandler
# from server.room_url_generator import RoomUrlGenerator
# from server.request_handler import RequestHandler
# from server.response_generator import ResponseGenerator
# from server.request_parser import RequestParser
# from server.fubble_rtc_pb2 import RtcMessageRequest

def create_app(test_config=None):
    # create and configure the app
    app = Flask(__name__, instance_relative_config=True)
    app.config.from_mapping(
        SECRET_KEY='dev',
        DATABASE=os.path.join(app.instance_path, 'server.db'),
    )

    if test_config is None:
        # load the instance config, if it exists, when not testing
        app.config.from_pyfile('config.py', silent=True)
    else:
        # load the test config if passed in
        app.config.from_mapping(test_config)

    # ensure the instance folder exists
    try:
        os.makedirs(app.instance_path)
    except OSError:
        pass

    room_url_generator = RoomUrlGenerator("https://rtc.fubble.io/")
    room_handler = RoomHandler(room_url_generator)
    response_generator = ResponseGenerator()
    request_handler = RequestHandler(response_generator, room_handler)
    request_parser = RequestParser()

    # a simple page that says hello
    @app.route('/')
    def handle():
        return "hello world"
        # deserialised = RtcMessageRequest()
        # deserialised.ParseFromString(request.data)
        # type_, data = request_parser.parse(deserialised)
        # return request_handler.handle(type_, data)

    @app.errorhandler(RoomHandler.NotFoundError)
    def handle_room_not_found(error):
        return Response()

    return app

