import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import io.fubble 1.0

Item {

    id: roomContainer
    property RoomModel room
    property var title: roomContainer.room.name
    property alias chat: chat
    property alias overview: overview
    property bool videosAvailable: room.videosAvailable

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

    Connections {
        target: room
        onVideosAvailableChanged: {
            videosAvailable = room.videosAvailable
        }
    }

    VideoWall {
        id: videoWall
        roomModel: roomContainer.room
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: videosAvailable ? overviewContainer.right : undefined
        anchors.margins: 10
        anchors.right: videosAvailable ? chatContainer.left : undefined
        width: !videosAvailable ? 0 : undefined
    }

    CustomRectangle {
        id: chatContainer
        lBorderwidth: videosAvailable ? 1 : 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color: Style.current.background
        anchors.left: !videosAvailable ? overviewContainer.right : undefined

        borderColor: Style.current.foreground
        width: chat.chatVisible && videosAvailable ? chat.chatWidth : 0

        Behavior on width {
            PropertyAnimation {
                id: chatAnimation
            }
        }

        Chat {
            id: chat
            anchors.fill: parent
            chatModel: room.chat
            chatParticipants: room.participants
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

