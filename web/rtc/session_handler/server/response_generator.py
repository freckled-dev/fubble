from server.fubble_rtc_pb2 import RtcMessageResponse, RoomInformation

class ResponseGenerator:
    def generateRoomInformation(self, url):
        return RoomInformation(rtcServer=url)

