import QtQuick 2.0

QtObject {

    property int userAudioInputDeviceIndex: 0
    property int userAudioOutputDeviceIndex
    property int userVideoDeviceIndex

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
}
