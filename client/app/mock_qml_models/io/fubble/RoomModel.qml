import QtQml 2.0

QtObject {
    property string name: "room name not set"
    property list<QtObject> participants: [
        QtObject {
            property var participant: ParticipantModel {
                name: "first"
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: "second"
                video_source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: "third"
                video_source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: "fourth"
            }
        }
    ]
}
