import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import io.fubble 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 640
    minimumHeight: 480
    title: "fubble"
    id: container
    property JoinModel joinModel: joinModelFromCpp

    header: Header {
        title: stack.currentItem.title
    }
    StackView {
        id: stack
        initialItem: joinComponent
        anchors.fill: parent
        focus: true
    }

    Component {
        id: joinComponent
        Join {
            joinModel: container.joinModel
            onJoined: {
                stack.push(roomComponent, {
                               "room": room
                           })
            }
        }
    }

    Component {
        id: roomComponent
        Room {}
    }
}
