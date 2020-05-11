import QtQuick 2.0

QtObject {
    id: chatContainer

    property var messages: ListModel {
        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962085
            message: "Hello World! How are you?"

            // message, join, leave, kick
            type: "message"
        }
    }

    function sendMessage(message) {
        var currentTimeStamp = new Date().getTime()

        messages.append({
                            "message": message,
                            "name": "Tamrielle",
                            "type": "message",
                            "own": true,
                            "timestamp": currentTimeStamp
                        })
    }
}
