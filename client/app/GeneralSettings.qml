import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12
import Qt.labs.settings 1.0
import "."

Rectangle {
    id: rectangle
    color: Style.current.transparent
    radius: 5
    property alias settings: settings
    border.width: 1
    border.color: Style.current.white

    Settings {
        id: settings
        property alias darkMode: darkModeSwitch.checked
        property alias showRoomHistory: roomHistorySwitch.checked
    }

    Item {
        id: settingItems

        Label {
            id: uiTitle
            text: qsTr("User Interface")
            anchors.topMargin: 20
            font.pointSize: Style.current.largeTextPointSize
            width: settingsDialog.availableWidth
        }

        Switch {
            id: darkModeSwitch
            anchors.top: uiTitle.bottom
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

        Switch {
            id: roomHistorySwitch
            anchors.top: darkModeSwitch.bottom
            text: qsTr("Show the room history")
            checked: true || settings.saveRoomHistory
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

