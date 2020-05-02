import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: container
    visible: true
    width: 1024
    height: 800
    minimumWidth: 800
    minimumHeight: 600
    title: "Fubble"
    property JoinModel joinModel: joinModelFromCpp
    property LeaveModel leaveModel: leaveModelFromCpp
    property bool shutdown: false
    property int fubbleState: FubbleApplication.FubbleState.Login

    // theme
    Material.primary: Style.current.primary
    Material.accent: Style.current.accent
    Material.background: Style.current.background
    Material.foreground: Style.current.foreground

    enum FubbleState {
        Login,
        Leaving,
        Room
    }

    header: Header {
        id: header
        title: stack.currentItem.title
        stackView: stack
        leave: leave
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
            id: join
            joinModel: container.joinModel
            onJoined: {
                stack.push(roomComponent, {
                               "room": room
                           })
                fubbleState = FubbleApplication.FubbleState.Room
            }
        }
    }

    onClosing: {
        if (fubbleState === FubbleApplication.FubbleState.Room) {
            shutdown = true
            close.accepted = false
            leave.showForceButton = true
            leave.open()
            return
        }

        close.accepted = true
    }

    Leave {
        id: leave
        leaveModel: container.leaveModel
        showForceButton: true
        onLeft: {
            stack.pop()
            stack.currentItem.setGuiEnabled(true)
            if (shutdown) {
                close()
            }
        }
        onLeaving: {
            fubbleState = FubbleApplication.FubbleState.Leaving
        }
        onForceShutdown: {
            container.close()
        }
    }

    Component {
        id: roomComponent
        Room {}
    }
}
