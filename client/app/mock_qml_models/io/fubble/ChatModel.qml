import QtQuick 2.0

QtObject {
    id: chatContainer

    property var messages: ListModel {
        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962085
            message: "Hey Wifey! How are you?"

            // message, join, leave, kick
            type: "message"
        }
        ListElement {
            name: "Susi Smith"
            own: false
            timestamp: 1588754962085
            message: "Hey Hubby! Doing the dishes and you? asdasdasddas<br><br> asd<br> asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  asdasdasddas asd asdas  "

            // message, join, leave, kick
            type: "message"
        }

        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962085
            message: "Making money!"

            // message, join, leave, kick
            type: "message"
        }

        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962085
            message: "Soon we can buy a house!"

            // message, join, leave, kick
            type: "message"
        }

        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962090

            // message, join, leave, kick
            type: "leave"
        }

        ListElement {
            name: "Horst"
            own: true
            timestamp: 1588754962090

            // message, join, leave, kick
            type: "join"
        }

        ListElement {
            name: "Horst"
            own: true
            timestamp: 1588754962085
            message: "Hi guys!"

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
