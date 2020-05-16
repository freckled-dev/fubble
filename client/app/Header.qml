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
    Material.foreground: Style.current.buttonTextColor

    ToolButton {
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
        }
    }

    ToolButton {
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

