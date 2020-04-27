import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.3
import Qt.labs.settings 1.0

Dialog {
    id: settingsDialog
    modal: true
    focus: true
    x: (container.width - width) / 2
    y: container.height / 6
    width: Math.min(container.width, container.height) / 3 * 2
    contentHeight: settingsContainer.height
    Material.foreground: Style.current.foreground

    property alias darkMode: settings.darkMode

    Settings {
        id: settings
        property alias darkMode: darkModeSwitch.checked
    }

    Column {
        id: settingsContainer
        height: 280
        spacing: 10
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

