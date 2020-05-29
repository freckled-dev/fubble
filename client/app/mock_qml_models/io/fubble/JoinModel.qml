import QtQuick 2.0
import QtMultimedia 5.0

QtObject {
    id: container

    property MediaPlayer video
    property bool videoAvailable: video != null
    property RoomModel model


    /*MediaPlayer {
        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
        autoPlay: true
    }*/
    property var timer: Timer {
        id: timer
        interval: 5000
        running: false
        repeat: false
        onTriggered: joined(model)
    }

    signal joined(RoomModel room)
    signal join_failed

    function join(room, name) {
        var component = Qt.createComponent("RoomModel.qml")
        var roomModel = component.createObject(container)
        roomModel.name = room
        model = roomModel
        timer.start()
    }
}
