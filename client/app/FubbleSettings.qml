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
    width: 450
    height: 420
    padding: 0

    property alias darkMode: settings.darkMode
    property alias joinSound: settings.joinSound
    property alias leaveSound: settings.leaveSound
    property alias errorSound: settings.errorSound
    property alias showRoomHistory: settings.showRoomHistory

    Settings {
        id: settings
        property alias darkMode: darkModeSwitch.checked
        property alias joinSound: joinSoundSwitch.checked
        property alias leaveSound: leaveSoundSwitch.checked
        property alias errorSound: errorSoundSwitch.checked
        property alias showRoomHistory: roomHistorySwitch.checked
    }

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        border.width: 1
        border.color: Style.current.white
        anchors.fill: parent

        Item {
            anchors.topMargin: 20
            anchors.bottomMargin: 10
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.fill: parent

            Label {
                id: settingsTitle
                width: 72
                anchors.top: parent.Top
                text: qsTr("Settings")
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: Style.current.subHeaderPointSize
            }

            Label {
                id: uiTitle
                anchors.top: settingsTitle.bottom
                text: qsTr("User Interface")
                anchors.topMargin: 20
                font.pointSize: Style.current.subHeaderPointSize
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
                text: qsTr("Audio")
                anchors.topMargin: 20
                anchors.top: roomHistorySwitch.bottom
                font.pointSize: Style.current.subHeaderPointSize
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
}

/*##^##
Designer {
    D{i:1;anchors_height:400;anchors_width:509;anchors_x:49;anchors_y:40}
}
##^##*/

