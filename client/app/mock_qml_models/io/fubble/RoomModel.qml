import QtQml 2.0

QtObject {
    property string name: "room name not set"
    property var chat: ChatModel {}

    property var ownParticipant: participants[0].participant

    property list<QtObject> participants: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("First")
                own: true
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Second")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Third")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Fourth")
            }
        }
    ]

    property list<QtObject> participantsWithVideo: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("second")
                video_source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
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
                video_source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
            }
        }
    ]
}
