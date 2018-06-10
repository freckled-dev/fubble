from concurrent import futures
import time
import grpc
import server.web_rtc_pb2
import server.web_rtc_pb2_grpc
from server.request_handler import RequestHandler
from server.response_generator import ResponseGenerator
from server.room_handler import RoomHandler
from server.room_url_generator import RoomUrlGenerator

class ServerGrpc(server.web_rtc_pb2_grpc.RtcServicer):
    def __init__(self, request_handler):
        self.request_handler = request_handler

    def JoinRoom(self, request, context):
        return self.request_handler.handle_join_room(request)

def start_server():
    response_generator = ResponseGenerator()
    url_generator = RoomUrlGenerator("https://rtc.fubble.io/")
    room_handler = RoomHandler(url_generator)
    request_handler = RequestHandler(response_generator, room_handler)

    grpc_server = ServerGrpc(request_handler)
    server_ = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    server.web_rtc_pb2_grpc.add_RtcServicer_to_server(grpc_server, server_)
    server_.add_insecure_port('[::]:20051')
    server_.start()
    return server_

def serve(server_):
    ONE_DAY_IN_SECONDS = 60 * 60 * 24
    try:
        while True:
            time.sleep(ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server_.stop(0)

if __name__ == '__main__':
    server_ = start_server()
    serve(server_)


