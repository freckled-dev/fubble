import QtQuick 2.12

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
            name: "Toni"
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
            name: "Toni"
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
            name: "Toni"
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
