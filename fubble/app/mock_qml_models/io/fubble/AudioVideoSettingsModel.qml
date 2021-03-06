import QtQuick 2.15
import QtMultimedia 5.15

QtObject {

    property int userAudioInputDeviceIndex: 0
    property int userAudioOutputDeviceIndex
    property int userVideoDeviceIndex
    property bool watchForNewAudioDevices

    // Audio
    property var inputDevices: ListModel {
        property bool available: false
        //        ListElement {
        //            inputId: "1"
        //            name: "Headset"
        //        }
        //        ListElement {
        //            inputId: "2"
        //            name: "Internal Audio"
        //        }
    }

    property var outputDevices: ListModel {
        property bool available: false
        //        ListElement {
        //            inputId: "1"
        //            name: "Headset"
        //        }
        //        ListElement {
        //            inputId: "2"
        //            name: "Internal Audio"
        //        }
    }

    // Video
    property var videoDevices: ListModel {
        property bool available: false
        //        ListElement {
        //            inputId: "2"
        //            name: "Webcam"
        //        }
        //        ListElement {
        //            inputId: "2"
        //            name: "Internal Video"
        //        }
    }

    property MediaPlayer videoPreview: MediaPlayer {
        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
        autoPlay: true
    }
}
