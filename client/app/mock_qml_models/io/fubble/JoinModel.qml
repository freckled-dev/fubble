import QtQuick 2.0
import QtMultimedia 5.0

QtObject {
    id: container

    property MediaPlayer video
    property bool videoAvailable: video != null

    signal joined(RoomModel room)
    signal join_failed

    function join(roomName, nickName) {
        mockJoin(roomName)
    }

    function joinWithPassword(roomName, nickName, password) {
        mockJoin(roomName)
    }

    function mockJoin(roomName) {
        var component = Qt.createComponent("RoomModel.qml")
        var roomModel = component.createObject(container)
        roomModel.name = roomName

        joined(roomModel)
    }
}
