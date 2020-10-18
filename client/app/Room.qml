import QtMultimedia 5.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Material 2.14
import io.fubble 1.0

Item {

    id: roomContainer
    property RoomModel room
    property var title: roomContainer.room.name
    property alias chat: chatContainer
    property alias overview: overviewContainer
    property bool videosAvailable: room.videosAvailable

    FubbleActionButton {
        id: overviewShowIcon
        anchors.top: parent.top
        anchors.left: parent.left
        icon.source: Style.current.collapseImageRight
        buttonWidth: 25
        buttonHeight: 35
        toolTipText: qsTr("Show participant view")
        visible: false
        z: 100

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                overviewContainer.visible = true
                overviewShowIcon.visible = false
            }
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Overview {
            id: overviewContainer
            roomModel: roomContainer.room
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            overviewShowIcon: overviewShowIcon
        }

        Connections {
            target: room
            onVideosAvailableChanged: {
                videosAvailable = room.videosAvailable
                if (!videosAvailable) {
                    chat.visible = true
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
            SplitView.minimumWidth: 200
        }

        Chat {
            id: chatContainer
            chatModel: room.chat
            roomModel: room
            chatParticipants: room.participants
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            chatShowIcon: chatShowIcon
        }
    }

    FubbleActionButton {
        id: chatShowIcon
        anchors.top: parent.top
        anchors.right: parent.right
        icon.source: Style.current.collapseImageLeft
        buttonWidth: 25
        buttonHeight: 35
        toolTipText: qsTr("Show chat view")
        visible: false
        z: 100

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                chatContainer.visible = true
                chatShowIcon.visible = false
            }
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

