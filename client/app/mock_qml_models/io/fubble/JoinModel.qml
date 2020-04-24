import QtQuick 2.0
import QtMultimedia 5.0

QtObject {
    id: container
    property string name: "some name"
    property string room: "some room name"
    property var video: MediaPlayer {
        source: "file:///home/mlanner/Downloads/big_buck_bunny.webm"
        autoPlay: true
    }
    signal joined(RoomModel room)
    function join(room, name) {
        console.log("name:", name, "room:", room)
        var component = Qt.createComponent("RoomModel.qml")
        var roomModel = component.createObject(container)
        roomModel.name = room
        joined(roomModel)
    }
}
