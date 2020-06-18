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
                name: qsTr("First Longer name")
                own: true
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Second name")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Third Again")
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Fourth of May")
            }
        }
    ]

    property list<QtObject> participantsWithVideo: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("second")
            }
        }
    ]
}
