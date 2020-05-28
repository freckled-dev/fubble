import QtQuick 2.0

QtObject {
    id: chatContainer

    property int newMessages

    function resetNewMessages() {
        console.log("resetNewMessages")
    }

    property var messages: ListModel {
        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962085
            message: "Hello World! How are you?"

            // message, join, leave, kick
            type: "message"
        }
        ListElement {
            name: "Horst"
            timestamp: 1588754962085
            message: "Very good, thank you. And yourself?"

            // message, join, leave, kick
            type: "message"
        }
        ListElement {
            name: "Horst"
            timestamp: 1588754962085
            message: "https://www.google.at/"

            // message, join, leave, kick
            type: "message"
        }
        ListElement {
            name: "Tamrielle"
            own: true
            timestamp: 1588754962085
            message: "Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. "

            // message, join, leave, kick
            type: "message"
        }
        ListElement {
            name: "Tamrielle"
            timestamp: 1588754962085

            // message, join, leave, kick
            type: "leave"
        }

        ListElement {
            name: "Hugo"
            timestamp: 1588754962085

            // message, join, leave, kick
            type: "join"
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
