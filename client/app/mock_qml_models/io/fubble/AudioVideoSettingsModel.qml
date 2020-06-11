import QtQuick 2.0
import QtMultimedia 5.0

QtObject {

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
        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
        autoLoad: false
    }
}
