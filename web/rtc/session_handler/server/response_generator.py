from server.fubble_rtc_pb2 import RtcMessageResponse, RoomInformation
import google.protobuf.empty_pb2

class ResponseGenerator:
    def generateRoomInformation(self, url):
        return RoomInformation(rtcServer=url)

    def generateGoogleProtobufEmpty(self):
        return google.protobuf.empty_pb2.Empty()

