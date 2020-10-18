import QtQuick 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
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
    property alias notificationSettings: notification.settings
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
                text: qsTr("Sounds / Notifications")
                width: implicitWidth
            }

            TabButton {
                text: qsTr("Audio / Video")
                width: implicitWidth
            }

            onCurrentIndexChanged: {
                if (currentIndex === 0 || currentIndex === 1) {
                    stopVideo()
                } else if (currentIndex === 2) {
                    startVideo()
                }
            }
        }

        StackLayout {
            id: tabItem
            currentIndex: bar.currentIndex
            Layout.margins: 20

            GeneralSettings {
                id: general
            }
            NotificationSettings {
                id: notification
            }
            AudioVideoSettings {
                id: audioVideo
            }
        }
    }

    onClosed: {
        ownMediaModel.loopbackOwnVoice = false
        audioVideoModel.watchForNewAudioDevices = false
        stopVideo()
    }

    function stopVideo() {
        if (audioVideoModel.videoPreview != null) {
            audioVideoModel.videoPreview.stop()
        }
    }

    function startVideo() {
        if (ownMediaModel.videoAvailable && bar.currentIndex == 2) {
            audioVideoModel.videoPreview.play()
        }
    }

    onOpened: {
        audioVideoModel.watchForNewAudioDevices = true
        startVideo()
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:400;anchors_width:509;anchors_x:49;anchors_y:40}
}
##^##*/

