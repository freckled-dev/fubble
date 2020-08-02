import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.12
import Qt.labs.settings 1.0

ApplicationWindow {
    id: container
    title: qsTr("Fubble")

    // initial values - will be overwritten by users settings
    width: Math.min(Screen.width, 1024)
    height: Math.min(Screen.height, 768)
    x: 0
    y: 0

    minimumWidth: 800
    minimumHeight: 600
    visible: true
    visibility: "Windowed"

    property JoinModel joinModel: joinModelFromCpp
    property LeaveModel leaveModel: leaveModelFromCpp
    property ErrorModel errorModel: errorModelFromCpp
    property UtilsModel utilsModel: utilsModelFromCpp
    property ShareDesktopModel shareDesktopModel: shareDesktopModelFromCpp
    property AudioVideoSettingsModel audioVideoModel: audioVideoModelFromCpp
    property OwnMediaModel ownMediaModel: ownMediaModelFromCpp

    property bool shutdown: false

    // Login, Joining, Leaving, Room
    property string fubbleState: "Login"

    property var currentRoomInfo

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
        leave: leavePopup
    }

    Settings {
        property alias x: container.x
        property alias y: container.y
        property alias width: container.width
        property alias height: container.height
        property alias visibility: container.visibility
    }

    StackView {
        id: stack
        initialItem: join
        anchors.fill: parent
        focus: true
    }

    Join {
        id: join
        joinModel: container.joinModel
        onJoined: {
            stack.push(roomComponent, {
                           "room": room
                       })
            fubbleState = "Room"

            currentRoomInfo = {
                "roomName": join.roomName,
                "enterTime": new Date().getTime()
            }

            header.timer.start()
        }
    }

    onClosing: {
        if (fubbleState === "Room") {
            shutdown = true
            close.accepted = false
            leavePopup.showForceButton = true
            leavePopup.open()
            return
        }

        close.accepted = true
    }

    ProgressPopup {
        id: leavePopup
        leaveModel: container.leaveModel
        showForceButton: true
        progressText: qsTr("Leaving the room...")
        isLeavePopup: true

        onLeft: {
            stack.pop()
            stack.currentItem.setGuiEnabled(true)
            if (shutdown) {
                container.close()
            }

            // seconds spent in the room
            var duration = (new Date().getTime(
                                ) - currentRoomInfo.enterTime) / 1000
            var durationRounded = Math.round(duration)
            currentRoomInfo.duration = durationRounded
            join.history.addRoomToHistory(currentRoomInfo)
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
        Room {

            Connections {
                target: header
                onToggleChat: {
                    chat.chatVisible = !chat.chatVisible
                }
                onToggleOverview: {
                    overview.overviewVisible = !overview.overviewVisible
                }
            }
        }
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
        if (header.fubbleSettings.generalSettings.joinSound) {
            joinSound.play()
        }
    }

    function playLeaveSound() {
        if (header.fubbleSettings.generalSettings.leaveSound) {
            leaveSound.play()
        }
    }

    function playErrorSound() {
        if (header.fubbleSettings.generalSettings.errorSound) {
            errorSound.play()
        }
    }
}
