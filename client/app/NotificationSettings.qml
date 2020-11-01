import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.0

Item {
    property alias settings: settings

    Settings {
        id: settings
        property alias joinSound: joinSoundSwitch.checked
        property alias messageSound: messageSoundSwitch.checked
        property alias leaveSound: leaveSoundSwitch.checked
        property alias errorSound: errorSoundSwitch.checked
        property alias showNotifications: notificationSwitch.checked
    }

    Item {

        Label {
            id: audioTitle
            text: qsTr("Sounds")
            anchors.topMargin: 20
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
            id: notificationTitle
            text: qsTr("Notifications")
            anchors.topMargin: 20
            anchors.top: errorSoundSwitch.bottom
            font.pointSize: Style.current.largeTextPointSize
        }

        Switch {
            id: notificationSwitch
            anchors.top: notificationTitle.bottom
            text: qsTr("Show notifications")
            checked: true || settings.showNotifications
        }
    }
}
