import QtQml 2.0

QtObject {
    property string name: "room name not set"
    property var chat: ChatModel {}
    property bool videosAvailable: true

    property var ownParticipant: participants[0].participant

    property int newParticipants

    function resetNewParticipants() {
        console.log("resetNewParticipants")
    }

    property list<QtObject> participants: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Sarah")
                own: true
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Konrad")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Toni")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Christine")
            }
        }
    ]

    property list<QtObject> participantsWithVideo: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Sarah")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Konrad")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Toni")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Christine")
            }
        }
    ]
}
