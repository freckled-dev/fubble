import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0

ApplicationWindow {
    visible: true
    width: 1024
    height: 800
    minimumWidth: 800
    minimumHeight: 600
    title: "fubble"
    id: container
    property JoinModel joinModel: joinModelFromCpp

    // theme
    Material.primary: Style.primary
    Material.accent: Style.accent

    header: Header {
        title: stack.currentItem.title
        Material.foreground: Style.foreground
    }
    StackView {
        id: stack
        initialItem: joinComponent
        anchors.fill: parent
        focus: true
    }

    // center app in display
    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2)
        setY(Screen.height / 2 - height / 2)
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
