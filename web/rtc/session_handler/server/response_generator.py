from server.fubble_rtc_pb2 import RtcMessageResponse, RoomInformation

class ResponseGenerator:
    def generateRoomInformation(self, url):
        response = self.generateMessageResponse()
        response.roomInformation.rtcServer = url
        return response

    def generateMessageResponse(self):
        return RtcMessageResponse()
