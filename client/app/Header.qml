import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0
import io.fubble 1.0

ToolBar {
    id: header
    property string title
    property alias settings: settings
    property var stackView
    property Leave leave
    signal toggleChat
    signal toggleOverview

    Material.foreground: Style.current.buttonTextColor

    ToolButton {
        id: backButton
        anchors.left: parent.left
        text: qsTr("‹")
        onClicked: {
            leave.showForceButton = false
            leave.open()
        }
        visible: header.isRoomView()
    }

    Label {
        id: titleLabel
        text: title
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: Style.current.subHeaderPointSize
        elide: Label.ElideRight
    }

    Image {
        id: copyImage
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: titleLabel.right
        anchors.leftMargin: 10
        source: Style.current.copyImage
        sourceSize.height: 30
        sourceSize.width: 30
        visible: header.isRoomView()

        FubbleToolTip {
            id: ttCopy
            text: qsTr("Copy room name")
            visible: maCopy.containsMouse
        }

        FubbleToolTip {
            id: ttCopied
            delay: 2
            visible: false
            text: qsTr("Room name copied to clipboard...")
        }

        Timer {
            id: timer
            triggeredOnStart: false
            interval: 1000
            onTriggered: ttCopied.visible = false
        }

        MouseArea {
            id: maCopy
            anchors.fill: parent
            hoverEnabled: true
            onExited: ttCopy.text = qsTr("Copy room name")
            onClicked: {
                utilsModel.copyToClipboard(title)
                ttCopied.visible = true
                timer.start()
            }

            onPressedChanged: {
                maCopy.pressed ? copyImage.source = Qt.binding(function () {
                    return Style.current.copyPressedImage
                }) : copyImage.source = Qt.binding(function () {
                    return Style.current.copyImage
                })
            }
        }
    }

    Image {
        id: overviewIcon
        anchors.verticalCenter: parent.verticalCenter
        sourceSize.width: 30
        sourceSize.height: 30
        anchors.right: chatIcon.visible ? chatIcon.left : moreButton.left
        anchors.rightMargin: 10
        source: Style.current.overviewImage
        visible: header.isRoomView()

        MouseArea {
            id: maOverview
            anchors.fill: parent
            hoverEnabled: true

            onPressedChanged: {
                maOverview.pressed ? overviewIcon.source = Qt.binding(
                                         function () {
                                             return Style.current.overviewPressedImage
                                         }) : overviewIcon.source = Qt.binding(
                                         function () {
                                             return Style.current.overviewImage
                                         })
            }

            onClicked: {
                toggleOverview()
            }
        }
    }

    Image {
        id: chatIcon
        anchors.verticalCenter: parent.verticalCenter
        sourceSize.width: 30
        sourceSize.height: 30
        anchors.right: moreButton.left
        anchors.rightMargin: 10
        source: Style.current.chatImage
        visible: header.isRoomView() && header.roomHasVideos()

        MouseArea {
            id: maChat
            anchors.fill: parent
            hoverEnabled: true

            onPressedChanged: {
                maChat.pressed ? chatIcon.source = Qt.binding(function () {
                    return Style.current.chatPressedImage
                }) : chatIcon.source = Qt.binding(function () {
                    return Style.current.chatImage
                })
            }

            onClicked: {
                toggleChat()
            }
        }
    }

    ToolButton {
        id: moreButton
        anchors.right: parent.right
        onClicked: optionsMenu.open()
        text: qsTr("⋮")

        Menu {
            Material.foreground: Style.current.foreground
            id: optionsMenu
            x: parent.width - width
            transformOrigin: Menu.TopRight
            MenuItem {
                text: qsTr("Settings")
                onTriggered: settings.open()
            }
            MenuItem {
                text: qsTr("About")
                onTriggered: aboutDialog.open()
            }
        }
    }

    function isRoomView() {
        if (stackView.currentItem.room) {
            return true
        }

        return false
    }

    function roomHasVideos() {
        var room = stackView.currentItem.room
        return room.videosAvailable
    }

    About {
        id: aboutDialog
    }

    Settings {
        id: settings
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

