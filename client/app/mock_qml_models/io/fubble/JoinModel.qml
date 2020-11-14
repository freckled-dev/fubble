import QtQuick 2.15

QtObject {
    id: container

    property var timer: Timer {
        id: timer
        interval: 50
        running: false
        repeat: false
        onTriggered: joined()
    }

    signal joined()
    signal join_failed

    function join(roomName, nickName) {
        mockJoin(roomName)
    }

    function joinWithPassword(roomName, nickName, password) {
        mockJoin(roomName)
    }

    function mockJoin(roomName) {
        timer.start()
    }
}
