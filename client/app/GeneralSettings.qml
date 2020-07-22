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
        property alias joinSound: joinSoundSwitch.checked
        property alias leaveSound: leaveSoundSwitch.checked
        property alias errorSound: errorSoundSwitch.checked
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

        Label {
            id: audioTitle
            text: qsTr("Sounds")
            anchors.topMargin: 20
            anchors.top: roomHistorySwitch.bottom
            font.pointSize: Style.current.largeTextPointSize
            width: settingsDialog.availableWidth
        }

        Label {
            id: audioDescription
            text: qsTr("Play a sound, when:")
            anchors.topMargin: 10
            anchors.top: audioTitle.bottom
            font.pointSize: Style.current.textPointSize
            width: settingsDialog.availableWidth
        }

        Switch {
            id: joinSoundSwitch
            anchors.top: audioDescription.bottom
            text: qsTr("user joins the room")
            checked: true || settings.joinSound
        }

        Switch {
            id: leaveSoundSwitch
            anchors.top: joinSoundSwitch.bottom
            text: qsTr("user leaves the room")
            checked: true || settings.leaveSound
        }

        Switch {
            id: errorSoundSwitch
            anchors.top: leaveSoundSwitch.bottom
            text: qsTr("an error occurred")
            checked: true || settings.errorSound
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

