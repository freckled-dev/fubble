import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import Qt.labs.settings 1.0
import "."

Popup {
    id: settingsDialog
    modal: true
    focus: true
    x: (container.width - width) / 2
    y: container.height / 6
    Material.foreground: Style.current.foreground
    width: 400
    height: 350
    topMargin: 1
    padding: 0

    property alias darkMode: settings.darkMode

    Settings {
        id: settings
        property alias darkMode: darkModeSwitch.checked
    }

    Rectangle {
        id: rectangle
        color: "#00000000"
        radius: 5
        border.width: 1
        border.color: "#ffffff"
        anchors.fill: parent
    }

    Column {
        id: settingsContainer
        height: 280
        anchors.topMargin: 20
        spacing: 20
        anchors.fill: parent

        Label {
            id: settingsTitle
            text: qsTr("Settings")
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 14
            width: settingsDialog.availableWidth
        }

        Switch {
            id: darkModeSwitch
            text: qsTr("Enable dark mode")
            checked: settings.darkMode
            onCheckedChanged: {
                if (darkModeSwitch.checked) {
                    Style.current = Style.themeDark
                } else {
                    Style.current = Style.themeLight
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:400;anchors_width:509;anchors_x:49;anchors_y:40}
}
##^##*/
