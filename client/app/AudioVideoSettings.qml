import QtQuick 2.0
import Qt.labs.settings 1.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtMultimedia 5.0
import "scripts/utils.js" as Utils

Item {
    id: element

    property alias settings: settings
    property bool isActive: false
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
            text: qsTr("Input Device")
            font.pointSize: Style.current.textPointSize
        }

        ComboBox {
            id: inputCombo
            Layout.fillWidth: true
            currentIndex: audioVideoModel.userAudioInputDeviceIndex
            textRole: "name"
            model: {
                if (audioVideoModel.inputDevices.count === 0) {
                    return [{
                                "name": "No input device found"
                            }]
                }
                return audioVideoModel.inputDevices
            }
        }

        Label {
            text: qsTr("Output Device")
            font.pointSize: Style.current.textPointSize
        }

        ComboBox {
            id: outputCombo
            Layout.fillWidth: true
            currentIndex: audioVideoModel.userAudioOutputDeviceIndex
            textRole: "name"
            model: {
                if (audioVideoModel.outputDevices.count === 0) {
                    return [{
                                "name": "No output device found"
                            }]
                }

                return audioVideoModel.outputDevices
            }
        }
    }

    Label {
        id: videoSettings
        anchors.top: audioGrid.bottom
        text: qsTr("Video")
        anchors.topMargin: 20
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
            model: {
                if (audioVideoModel.videoDevices.count === 0) {
                    return [{
                                "name": "No video device found"
                            }]
                }

                return audioVideoModel.videoDevices
            }
        }

        Label {
            text: qsTr("Preview")
            font.pointSize: Style.current.textPointSize
        }

        Rectangle {
            height: 200
            Layout.fillWidth: true

            Loader {
                anchors.fill: parent

                sourceComponent: {
                    if (demoMode) {
                        return demoPreviewComponent
                    }

                    if (audioVideoModel.videoPreview) {
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
                    id: videoPreview
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

    onIsActiveChanged: {
        if (isActive && audioVideoModel.videoPreview) {
            audioVideoModel.videoPreview.play()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
