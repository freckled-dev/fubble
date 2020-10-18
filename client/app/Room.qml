import QtMultimedia 5.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.14
import io.fubble 1.0

SplitView {

    id: roomContainer
    property RoomModel room
    property var title: roomContainer.room.name
    property alias chat: chatContainer
    property alias overview: overviewContainer
    property bool videosAvailable: room.videosAvailable

    orientation: Qt.Horizontal

    Overview {
        id: overviewContainer
        roomModel: roomContainer.room
        SplitView.preferredWidth: 300
    }

    Connections {
        target: room
        onVideosAvailableChanged: {
            videosAvailable = room.videosAvailable
            if (!videosAvailable) {
                chat.chatVisible = true
            }
        }
    }

    VideoWall {
        id: videoWall
        roomModel: roomContainer.room
        anchors.margins: 10
        SplitView.fillWidth: true
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    Chat {
        id: chatContainer
        chatModel: room.chat
        chatParticipants: room.participants
        SplitView.preferredWidth: 500
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

