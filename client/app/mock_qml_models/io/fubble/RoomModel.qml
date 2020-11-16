import QtQml 2.0
import QtMultimedia 5.15

QtObject {
    property string name: "room name not set"
    property var chat: ChatModel {}
    property bool videosAvailable: true

    property int newParticipants: 3

    function resetNewParticipants() {
        console.log("resetNewParticipants")
    }

    property list<QtObject> participants: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Vanessa")
                own: true
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
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Konrad")
            }
        }
    ]

    property list<QtObject> participantsWithVideo: [
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Vanessa")
                video: MediaPlayer {
                    source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    autoPlay: true
                }
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Toni")
                video: MediaPlayer {
                    source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    autoPlay: true
                }
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Christine")
                video: MediaPlayer {
                    source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    autoPlay: true
                }
            }
        },
        QtObject {
            property var participant: ParticipantModel {
                name: qsTr("Konrad")
                video: MediaPlayer {
                    source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    autoPlay: true
                }
            }
        }
    ]
}
