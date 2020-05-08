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

    RowLayout {
        spacing: 20
        anchors.fill: parent

        ToolButton {
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
            font.pointSize: Style.current.headerPointSize
            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        ToolButton {
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
