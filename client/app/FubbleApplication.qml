import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: container
    title: qsTr("Fubble")

    width: 1400
    height: 800
    minimumWidth: 800
    minimumHeight: 600
    visible: true

    screen: Qt.application.screens[0]
    x: screen.virtualX + (screen.width / 2 - width / 2)
    y: screen.virtualY + (screen.height / 2 - height / 2)

    property JoinModel joinModel: joinModelFromCpp
    property LeaveModel leaveModel: leaveModelFromCpp
    property ErrorModel errorModel: errorModelFromCpp

    property bool shutdown: false
    property string fubbleState: "Login"

    // theme
    Material.primary: Style.current.primary
    Material.accent: Style.current.accent
    Material.background: Style.current.background
    Material.foreground: Style.current.foreground

    font.family: verdanaFont.name + ", " + emojiOneFont.name
    font.pointSize: Style.current.textPointSize

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

    Component {
        id: joinComponent

        Join {
            id: join
            joinModel: container.joinModel
            onJoined: {
                stack.push(roomComponent, {
                               "room": room
                           })
                fubbleState = "Room"

                playJoinSound()
            }
        }
    }

    onClosing: {
        if (fubbleState === "Room") {
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
            playLeaveSound()
            stack.pop()
            stack.currentItem.setGuiEnabled(true)
            if (shutdown) {
                container.close()
            }
        }
        onLeaving: {
            fubbleState = "Leaving"
        }
        onForceShutdown: {
            container.close()
        }
    }

    FontLoader {
        id: emojiOneFont
        source: Style.current.emojiOneFontSrc
    }

    FontLoader {
        id: verdanaFont
        source: Style.current.verdanaFontSrc
    }

    Error {
        id: errorPopup
        errorModel: container.errorModel
    }

    Component {
        id: roomComponent
        Room {}
    }

    // audio effects
    SoundEffect {
        id: joinSound
        source: "sounds/join.wav"
    }

    SoundEffect {
        id: leaveSound
        source: "sounds/leave.wav"
    }

    SoundEffect {
        id: errorSound
        source: "sounds/error.wav"
    }

    function playJoinSound() {
        if (header.settings.joinSound) {
            joinSound.play()
        }
    }

    function playLeaveSound() {
        if (header.settings.leaveSound) {
            leaveSound.play()
        }
    }

    function playErrorSound() {
        if (header.settings.errorSound) {
            errorSound.play()
        }
    }
}
