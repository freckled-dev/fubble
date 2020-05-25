import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.0
import io.fubble 1.0

Item {

    id: roomContainer
    property RoomModel room
    property var title: roomContainer.room.name
    property alias chat: chat
    property alias overview: overview

    CustomRectangle {
        id: overviewContainer
        rBorderwidth: 1
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: Style.current.background
        anchors.left: parent.left
        borderColor: Style.current.foreground
        width: overview.overviewVisible ? overview.overviewWidth : 0

        Behavior on width {
            PropertyAnimation {
                id: overviewAnimation
            }
        }

        Overview {
            id: overview
            anchors.fill: parent
            roomModel: roomContainer.room
        }
    }

    VideoWall {
        id: videoWall
        roomModel: roomContainer.room
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: room.videosAvailable ? overviewContainer.right : undefined
        anchors.margins: 10
        anchors.right: room.videosAvailable ? chatContainer.left : undefined
        width: !room.videosAvailable ? 0 : undefined
    }

    CustomRectangle {
        id: chatContainer
        lBorderwidth: room.videosAvailable ? 1 : 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color: Style.current.background
        implicitWidth: chat.width
        anchors.left: !room.videosAvailable ? overviewContainer.right : undefined
        borderColor: Style.current.foreground
        width: chat.chatVisible ? chat.chatWidth : 0

        Behavior on width {
            PropertyAnimation {
                id: chatAnimation
            }
        }

        Chat {
            id: chat
            anchors.fill: parent
            chatModel: room.chat
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

