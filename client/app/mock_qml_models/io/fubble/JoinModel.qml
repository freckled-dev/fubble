import QtQuick 2.0
import QtMultimedia 5.0

QtObject {
    id: container
    property string name: "some name"
    property string room: "some room name"
    property MediaPlayer video
    property bool videoAvailable: video != null


    /*MediaPlayer {
        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
        autoPlay: true
    }*/
    signal joined(RoomModel room)
    function join(room, name) {
        var component = Qt.createComponent("RoomModel.qml")
        var roomModel = component.createObject(container)
        roomModel.name = room
        joined(roomModel)
    }
}
