from concurrent import futures
import time
import grpc
import server.web_rtc_pb2
import server.web_rtc_pb2_grpc

class ServerGrpc(server.web_rtc_pb2_grpc.RtcServicer):
    def JoinRoom(self, request, context):
        print(request.id)

def serve():
    server_ = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    server.web_rtc_pb2_grpc.add_RtcServicer_to_server(ServerGrpc(), server_)
    server_.add_insecure_port('[::]:20051')
    server_.start()
    ONE_DAY_IN_SECONDS = 60 * 60 * 24
    try:
        while True:
            time.sleep(ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server_.stop(0)

if __name__ == '__main__':
    serve()


