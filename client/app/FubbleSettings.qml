import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import "."

Popup {
    id: settingsDialog
    modal: true
    focus: true

    parent: overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    Material.foreground: Style.current.foreground
    padding: 0
    height: 650
    width: 500

    property alias bar: bar
    property alias generalSettings: general.settings
    property alias audioVideoSettings: audioVideo.settings

    ColumnLayout {
        id: shareColumnLayout
        anchors.right: parent.right
        anchors.left: parent.left

        Label {
            id: settingsTitle
            text: qsTr("Settings")
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.topMargin: 20
            font.pointSize: Style.current.subHeaderPointSize
        }

        TabBar {
            id: bar
            currentIndex: 0
            Layout.topMargin: 10
            Layout.rightMargin: 20
            Layout.leftMargin: 20

            TabButton {
                text: qsTr("General")
                width: implicitWidth
            }

            TabButton {
                text: qsTr("Audio / Video")
                width: implicitWidth
            }
        }

        StackLayout {
            id: tabItem
            currentIndex: bar.currentIndex
            Layout.margins: 20

            GeneralSettings {
                id: general
            }
            AudioVideoSettings {
                id: audioVideo
            }

            onCurrentIndexChanged: audioVideo.tabIsActive = currentIndex === 1
        }
    }

    onClosed: {
        ownMediaModel.loopbackOwnVoice = false
        audioVideo.tabIsActive = false
        bar.currentIndex = 0
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:400;anchors_width:509;anchors_x:49;anchors_y:40}
}
##^##*/

