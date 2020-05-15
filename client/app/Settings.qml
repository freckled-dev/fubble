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
    height: 400
    topMargin: 1
    padding: 10

    property alias darkMode: settings.darkMode
    property alias joinSound: settings.joinSound
    property alias leaveSound: settings.leaveSound
    property alias errorSound: settings.errorSound

    Settings {
        id: settings
        property alias darkMode: darkModeSwitch.checked
        property alias joinSound: joinSoundSwitch.checked
        property alias leaveSound: leaveSoundSwitch.checked
        property alias errorSound: errorSoundSwitch.checked
    }

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        border.width: 1
        border.color: Style.current.white
        anchors.fill: parent
    }

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
                Style.isLightTheme = false
                Style.current = Style.themeDark
            } else {
                Style.isLightTheme = true
                Style.current = Style.themeLight
            }
        }
    }

    Label {
        id: audioTitle
        text: qsTr("Audio")
        anchors.topMargin: 20
        anchors.top: darkModeSwitch.bottom
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
        checked: settings.joinSound
    }

    Switch {
        id: leaveSoundSwitch
        anchors.top: joinSoundSwitch.bottom
        text: qsTr("user leaves the room")
        checked: settings.leaveSound
    }

    Switch {
        id: errorSoundSwitch
        anchors.top: leaveSoundSwitch.bottom
        text: qsTr("an error occurred")
        checked: settings.errorSound
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:400;anchors_width:509;anchors_x:49;anchors_y:40}D{i:3;anchors_width:72}
}
##^##*/

