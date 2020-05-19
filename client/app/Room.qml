import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.0
import io.fubble 1.0

Item {

    id: layout
    property RoomModel room
    property var title: layout.room.name
    property int roomMargin: 20
    property alias chat: chat
    property alias overview: overview

    Overview {
        id: overview
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: roomMargin
        roomModel: layout.room
    }

    Rectangle {
        id: separator1
        color: Style.current.foreground
        border.width: 0
        width: 1
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: overview.right
        anchors.leftMargin: visible ? roomMargin : 0
        visible: overview.overviewVisible
    }

    VideoWall {
        id: videoWall
        roomModel: layout.room
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: separator1.right
        anchors.topMargin: roomMargin
        anchors.bottomMargin: roomMargin
        anchors.right: videoCount !== 0 ? separator2.left : undefined
        width: videoCount === 0 ? 0 : undefined
    }

    Rectangle {
        id: separator2
        color: Style.current.foreground
        border.width: 0
        width: 1
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: chat.left
        anchors.rightMargin: visible ? roomMargin : 0
        visible: chat.chatVisible && videoWall.videoCount !== 0
    }

    Chat {
        id: chat
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.topMargin: roomMargin
        anchors.bottomMargin: roomMargin
        anchors.rightMargin: roomMargin
        anchors.leftMargin: separator1.visible ? roomMargin : undefined
        anchors.left: !videoWall.visible ? separator1.right : undefined
        chatModel: room.chat
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

