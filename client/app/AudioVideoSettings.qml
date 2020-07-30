import QtQuick 2.12
import Qt.labs.settings 1.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtMultimedia 5.12
import "scripts/utils.js" as Utils

Item {
    id: element

    property alias settings: settings
    property bool tabIsActive: false
    property bool demoMode: Utils.isDemoMode()

    Settings {
        id: settings
    }

    Label {
        id: audioSettings
        text: qsTr("Audio")
        font.pointSize: Style.current.largeTextPointSize
    }

    GridLayout {
        id: audioGrid
        columnSpacing: 20
        anchors.top: audioSettings.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.topMargin: 10
        columns: 2

        Label {
            text: qsTr("Output Device")
            font.pointSize: Style.current.textPointSize
        }

        ComboBox {
            id: outputCombo
            Layout.fillWidth: true
            currentIndex: audioVideoModel.userAudioOutputDeviceIndex
            textRole: "name"
            onActivated: audioVideoModel.onAudioOutputDeviceActivated(index)
            enabled: audioVideoModel.outputDevices.available
            model: audioVideoModel.outputDevices
        }

        Label {
            text: qsTr("Input Device")
            font.pointSize: Style.current.textPointSize
        }

        ComboBox {
            id: inputCombo
            Layout.fillWidth: true
            currentIndex: audioVideoModel.userAudioInputDeviceIndex
            textRole: "name"
            onActivated: audioVideoModel.onAudioInputDeviceActivated(index)
            enabled: audioVideoModel.inputDevices.available
            model: audioVideoModel.inputDevices
        }

        Button {
            text: ownMediaModel.loopbackOwnVoice ? qsTr("Stop test") : qsTr(
                                                       "Test microphone")
            onClicked: ownMediaModel.loopbackOwnVoice = !ownMediaModel.loopbackOwnVoice
        }

        Connections {
            target: ownMediaModel
            onNewAudioTestLevel: {
                if (ownMediaModel.loopbackOwnVoice) {
                    audioChart.addNewAudioLevel(level)
                }
            }
        }

        AudioChart {
            id: audioChart
            Layout.bottomMargin: 2
            Layout.topMargin: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Label {
        id: videoSettings
        anchors.top: audioGrid.bottom
        text: qsTr("Video")
        anchors.topMargin: 30
        font.pointSize: Style.current.largeTextPointSize
    }

    GridLayout {
        id: videoGrid
        columnSpacing: 20
        anchors.top: videoSettings.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.topMargin: 10
        columns: 2

        Label {
            text: qsTr("Video Devices")
            font.pointSize: Style.current.textPointSize
        }

        ComboBox {
            id: videoCombo
            Layout.fillWidth: true
            currentIndex: audioVideoModel.userVideoDeviceIndex
            textRole: "name"
            onActivated: audioVideoModel.onVideoDeviceActivated(index)
            enabled: audioVideoModel.videoDevices.available
            model: audioVideoModel.videoDevices
        }

        Label {
            text: qsTr("Preview")
            font.pointSize: Style.current.textPointSize
        }

        Rectangle {
            height: 200
            Layout.fillWidth: true

            Loader {
                id: previewLoader
                anchors.fill: parent

                sourceComponent: {
                    if (demoMode) {
                        return demoPreviewComponent
                    }

                    if (ownMediaModel.videoAvailable) {
                        return videoPreviewComponent
                    }

                    return noPreviewComponent
                }
            }

            Component {
                id: noPreviewComponent
                NoVideo {
                    headerLabelText: qsTr("No camera found")
                    infoLabelText: qsTr("Please check your camera and operating system settings")
                }
            }

            Component {
                id: videoPreviewComponent

                VideoOutput {
                    anchors.fill: parent
                    source: audioVideoModel.videoPreview
                    fillMode: VideoOutput.PreserveAspectCrop
                }
            }

            Component {
                id: demoPreviewComponent

                Image {
                    source: Style.current.demoImagesPath + "Sarah.jpg"
                    fillMode: Image.PreserveAspectCrop
                }
            }
        }
    }

    onTabIsActiveChanged: {
        if (tabIsActive && ownMediaModel.videoAvailable) {
            audioVideoModel.videoPreview.play()
        }

        if (!tabIsActive) {
            audioVideoModel.videoPreview.stop()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

