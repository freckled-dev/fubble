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
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.margins: 10
    }

    // remove this once we have the new layout
    Component.onCompleted: {
        container.width = 1400
        setX(Screen.width / 2 - container.width / 2)
        setY(Screen.height / 2 - container.height / 2)
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

