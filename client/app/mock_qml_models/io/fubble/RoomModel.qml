import QtQml 2.0

QtObject {
    property list<QtObject> participants: [
        QtObject {
            property ParticipantModel participant: {
                name: "first"
            }
        },
        QtObject {
            property ParticipantModel participant: {
                name: "second"
            }
        }
    ]
}
