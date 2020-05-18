import QtQuick 2.0
import QtMultimedia 5.0

QtObject {
    id: container

    property MediaPlayer video
    property bool videoAvailable: video != null


    /*MediaPlayer {
        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
        autoPlay: true
    }*/
    signal joined(RoomModel room)
    signal join_failed

    function join(room, name) {
        var component = Qt.createComponent("RoomModel.qml")
        var roomModel = component.createObject(container)
        roomModel.name = room
        joined(roomModel)
    }
}
