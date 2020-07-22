import QtQuick 2.12

QtObject {
    id: container

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

        joined(roomModel)
    }
}
