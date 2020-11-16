import QtMultimedia 5.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import io.fubble 1.0
import QtQuick.Controls.Material 2.15
import Qt.labs.settings 1.0

ApplicationWindow {
    id: container
    title: qsTr("Fubble")

    // initial values - will be overwritten by users settings
    width: Math.min(Screen.width, 1024)
    height: Math.min(Screen.height, 768)
    x: 0
    y: 0
    Component.onCompleted: ensureValidWindowPosition()

    minimumWidth: 800
    minimumHeight: 600
    visible: true
    visibility: "Windowed"

    property JoinModel joinModel: joinModelFromCpp
    property LeaveModel leaveModel: leaveModelFromCpp
    property ErrorModel errorModel: errorModelFromCpp
    property UtilsModel utilsModel: utilsModelFromCpp
    property RoomsModel roomsModel: roomsModelFromCpp
    property ShareDesktopModel shareDesktopModel: shareDesktopModelFromCpp
    property AudioVideoSettingsModel audioVideoModel: audioVideoModelFromCpp
    property LanguageSettingsModel languageModel: languageModelFromCpp
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
        settingsDialog: settingsDialog
        aboutDialog: aboutDialog
    }

    Settings {
        id: settings
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
        settingsDialog: settingsDialog
        onJoined: {
            stack.push(roomComponent, {
                           "room": roomsModel.room
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
        // https://stackoverflow.com/questions/51810358/how-to-quit-the-c-application-in-qt-qml
        Qt.callLater(Qt.quit)
    }

    SystemTrayIcon {
        id: trayIcon
        visible: true
        iconSource: Style.current.logoImage
        menu: Menu {
            visible: false
            MenuItem {
                text: qsTr("Settings")
                onTriggered: {
                    aboutDialog.close()
                    settingsDialog.open()
                }
            }

            MenuItem {
                text: qsTr("About")
                onTriggered: {
                    settingsDialog.close()
                    aboutDialog.open()
                }
            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: {
                    container.close()
                }
            }
        }

        onActivated: {
            container.show()
            container.raise()
            container.requestActivate()
        }
    }

    FubbleSettings {
        id: settingsDialog
    }

    About {
        id: aboutDialog
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

    SoundEffect {
        id: messageSound
        source: "sounds/message.wav"
    }

    function playJoinSound() {
        var isJoinSoundEnabled = settingsDialog.notificationSettings.joinSound
        if (isJoinSoundEnabled) {
            joinSound.play()
        }
    }

    function playMessageSound() {
        var isMessageSoundEnabled = settingsDialog.notificationSettings.messageSound
        if (isMessageSoundEnabled) {
            messageSound.play()
        }
    }

    function playLeaveSound() {
        var isLeaveSoundEnabeled = settingsDialog.notificationSettings.leaveSound
        if (isLeaveSoundEnabeled) {
            leaveSound.play()
        }
    }

    function playErrorSound() {
        var isErrorSoundEnabled = settingsDialog.notificationSettings.errorSound
        if (isErrorSoundEnabled) {
            errorSound.play()
        }
    }

    function ensureValidWindowPosition() {
        if (Qt.platform.os == "windows") {
            return
        }

        console.log(Screen.width)
        console.log(Screen.height)
        console.log("x: " + settings.x)
        console.log("y: " + settings.y)
        //        window.x = (savedScreenLayout) ? settings.x : Screen.width / 2 - window.width / 2
        //        window.y = (savedScreenLayout) ? settings.y : Screen.height / 2 - window.height / 2
    }
}
