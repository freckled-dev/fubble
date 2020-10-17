import QtQuick 2.14

QtObject {
    id: container

    property var timer: Timer {
        property RoomModel roomModel
        id: timer
        interval: 2000
        running: false
        repeat: false
        onTriggered: joined(roomModel)
    }

    signal joined(QtObject room)
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

        timer.roomModel = roomModel
        timer.start()
    }
}
