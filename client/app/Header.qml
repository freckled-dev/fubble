import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0
import io.fubble 1.0
import "scripts/utils.js" as Utils

ToolBar {
    id: header
    property string title
    property alias settings: settings
    property var stackView
    property ProgressPopup leave
    signal toggleChat
    signal toggleOverview

    property alias timer: roomTimer

    // icon indicators
    property RoomModel room
    property ChatModel chat

    Material.foreground: Style.current.buttonTextColor

    ToolButton {
        id: backButton
        anchors.left: parent.left
        text: qsTr("‹")
        onClicked: {
            console.log("clicked")
            leave.showForceButton = false
            leave.open()
        }
        visible: header.isRoomView()
    }

    MouseArea {
        id: maHeader
        anchors.left: backButton.right
        anchors.right: overviewIconLoader.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        hoverEnabled: true
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

    Label {
        id: roomTimeLabel
        property int timePassed: 0
        text: Utils.toHHMMSS(timePassed)
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: overviewIconLoader.left
        anchors.rightMargin: 40
        font.pointSize: Style.current.textPointSize
        elide: Label.ElideRight
        visible: header.isRoomView() && maHeader.containsMouse

        Timer {
            id: roomTimer
            interval: 1000
            running: true
            repeat: true
            onTriggered: roomTimeLabel.timePassed += 1
        }
    }

    Loader {
        id: overviewIconLoader
        sourceComponent: header.isRoomView() ? overviewIconComponent : undefined

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: chatIconLoader.visible ? chatIconLoader.left : moreButton.left
        anchors.rightMargin: 10
    }

    Component {
        id: overviewIconComponent

        Image {
            id: overviewIcon
            sourceSize.width: 30
            sourceSize.height: 30

            source: room.newParticipants ? Style.current.overviewNewImage : Style.current.overviewImage

            MouseArea {
                id: maOverview
                anchors.fill: parent
                hoverEnabled: true

                onPressedChanged: {
                    maOverview.pressed ? overviewIcon.source = Qt.binding(
                                             function () {
                                                 return room.newParticipants ? Style.current.overviewNewPressedImage : Style.current.overviewPressedImage
                                             }) : overviewIcon.source = Qt.binding(
                                             function () {
                                                 return room.newParticipants ? Style.current.overviewNewImage : Style.current.overviewImage
                                             })
                }

                onClicked: {
                    toggleOverview()
                }
            }
        }
    }

    Loader {
        id: chatIconLoader
        sourceComponent: header.isRoomView() ? chatIconComponent : undefined

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: moreButton.left
        anchors.rightMargin: 10

        visible: header.isRoomView() && header.roomHasVideos()
    }

    Component {
        id: chatIconComponent

        Item {
            id: chatIconContainer

            implicitHeight: 30
            implicitWidth: 30

            Image {
                id: chatIcon
                anchors.fill: parent
                sourceSize.width: 30
                sourceSize.height: 30
                source: chat.newMessages
                        > 0 ? Style.current.chatNewMessageImage : Style.current.chatImage

                MouseArea {
                    id: maChat
                    anchors.fill: parent
                    hoverEnabled: true

                    onPressedChanged: {
                        maChat.pressed ? chatIcon.source = Qt.binding(
                                             function () {
                                                 return chat.newMessages > 0 ? Style.current.chatNewMessageImage : Style.current.chatPressedImage
                                             }) : chatIcon.source = Qt.binding(
                                             function () {
                                                 return chat.newMessages > 0 ? Style.current.chatNewMessageImage : Style.current.chatImage
                                             })
                    }

                    onClicked: {
                        toggleChat()
                    }
                }
            }

            Label {
                id: chatNumber
                visible: chat.newMessages > 0
                Material.foreground: Style.current.accentLight
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 8
                text: chat.newMessages
                font.bold: true
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
            room = stackView.currentItem.room
            chat = stackView.currentItem.room.chat
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

    FubbleSettings {
        id: settings
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

