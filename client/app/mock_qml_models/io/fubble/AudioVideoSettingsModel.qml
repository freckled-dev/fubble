import QtQuick 2.0
import QtMultimedia 5.0

QtObject {

    property int userAudioInputDeviceIndex: 1
    property int userAudioOutputDeviceIndex
    property int userVideoDeviceIndex

    // Audio
    property var inputDevices: ListModel {
        ListElement {
            inputId: "1"
            name: "Headset"
        }
        ListElement {
            inputId: "2"
            name: "Internal Audio"
        }
    }

    property var outputDevices: ListModel {
        ListElement {
            inputId: "1"
            name: "Headset"
        }
        ListElement {
            inputId: "2"
            name: "Internal Audio"
        }
    }

    // Video
    property var videoDevices: ListModel {

        ListElement {
            inputId: "2"
            name: "Webcam"
        }
        ListElement {
            inputId: "2"
            name: "Internal Video"
        }
    }

    property var videoPreview: MediaPlayer {
        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
        autoLoad: false
    }
}
