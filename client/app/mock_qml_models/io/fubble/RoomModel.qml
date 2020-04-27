import QtQml 2.0

QtObject {
    property string name: "room name not set"
    property list<QtObject> participants: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("first")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("second")
                video_source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("third")
                video_source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("fourth")
            }
        }
    ]
}
