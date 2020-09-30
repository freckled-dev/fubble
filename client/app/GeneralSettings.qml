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
        property alias messageSound: messageSoundSwitch.checked
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
            text: qsTr("A user joins the room")
            checked: true || settings.joinSound
        }

        Switch {
            id: leaveSoundSwitch
            anchors.top: joinSoundSwitch.bottom
            text: qsTr("A user leaves the room")
            checked: true || settings.leaveSound
        }

        Switch {
            id: messageSoundSwitch
            anchors.top: leaveSoundSwitch.bottom
            text: qsTr("A chat message is received")
            checked: true || settings.messageSound
        }

        Switch {
            id: errorSoundSwitch
            anchors.top: messageSoundSwitch.bottom
            text: qsTr("An error occurred")
            checked: true || settings.errorSound
        }

        Label {
            id: languageTitle
            text: qsTr("Language")
            anchors.topMargin: 20
            anchors.top: errorSoundSwitch.bottom
            font.pointSize: Style.current.largeTextPointSize
            width: settingsDialog.availableWidth
        }

        ComboBox {
            id: videoCombo
            // does not work. i guess because the property is read only
            // currentIndex: languageModel.languageIndex
            onCurrentIndexChanged: languageModel.languageIndex = currentIndex
            textRole: "description"
            anchors.top: languageTitle.bottom
            model: languageModel.languages
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

