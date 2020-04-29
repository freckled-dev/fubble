import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0

RowLayout {

    id: layout
    property RoomModel room
    property var title: layout.room.name

    Overview {
        id: overview
        width: 300
        Layout.fillHeight: true
    }

    VideoWall {
        id: videoWall
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

