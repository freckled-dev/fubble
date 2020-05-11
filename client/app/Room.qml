import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.0
import io.fubble 1.0

RowLayout {

    id: layout
    property RoomModel room
    property var title: layout.room.name
    spacing: 0

    Overview {
        id: overview
        width: 250
        roomModel: layout.room
        Layout.fillHeight: true
    }

    Rectangle {
        width: 1
        color: Style.current.foreground
        Layout.fillHeight: true
    }

    VideoWall {
        id: videoWall
        roomModel: layout.room
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

    Chat {
        id: chat
        chatModel: room.chat
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.margins: 10
        visible: videoWall.videoCount === 0
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

