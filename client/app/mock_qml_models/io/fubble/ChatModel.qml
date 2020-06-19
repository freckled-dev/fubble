import QtQuick 2.0

QtObject {
    id: chatContainer

    property int newMessages

    function resetNewMessages() {
        console.log("resetNewMessages")
    }

    // message, join, leave, kick
    property var messages: ListModel {
        ListElement {
            participantId: "1"
            own: true
            name: "Sarah"
            type: "join"
        }
        ListElement {
            participantId: "2"
            name: "Horst"
            type: "join"
        }
        ListElement {
            participantId: "1"
            name: "Sarah"
            own: true
            timestamp: 1592567700000
            message: "Hello Guys! How are you today?"
            type: "message"
        }
        ListElement {
            participantId: "2"
            name: "Horst"
            timestamp: 1592567780000
            message: "Hi Sarah, doing great today, thanks 😃 And yourself?"
            type: "message"
        }
        ListElement {
            participantId: "1"
            name: "Sarah"
            timestamp: 1592567935000
            own: true
            message: "Had a lovely morning. I treated myself to cinnamon rolls and a matcha latte 🍵"
            type: "message"
        }
        ListElement {
            participantId: "2"
            name: "Horst"
            timestamp: 1592568005000
            message: "Oh wow, that sounds delicious!"
            type: "message"
        }
        ListElement {
            participantId: "3"
            name: "Christine"
            type: "join"
        }
        ListElement {
            participantId: "3"
            name: "Christine"
            timestamp: 1592568030000
            type: "message"
            message: "Hi everyone"
        }
        ListElement {
            participantId: "1"
            name: "Sarah"
            timestamp: 1592568030000
            own: true
            type: "message"
            message: "Hey Chrissie!"
        }
        ListElement {
            participantId: "4"
            name: "Konrad"
            type: "join"
        }
        ListElement {
            participantId: "4"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi all! Sorry for being late 😊"
        }

        ListElement {
            participantId: "5"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "6"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "7"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "8"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "9"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "10"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "11"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "12"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "13"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "14"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "15"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "16"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "17"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "18"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "19"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
        }
        ListElement {
            participantId: "20"
            name: "Konrad"
            timestamp: 1592568450000
            type: "message"
            message: "Hi"
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
