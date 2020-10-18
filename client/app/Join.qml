import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import QtMultimedia 5.14
import io.fubble 1.0
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.14
import "."
import "scripts/utils.js" as Utils

FocusScope {
    id: joinRoomContainer
    property var title: qsTr("Join a room")
    property JoinModel joinModel
    property bool guiEnabled: true
    property FubbleSettings settingsDialog
    property alias history: history
    property alias roomName: roomTextField.text
    property bool demoMode: Utils.isDemoMode()

    signal joined(RoomModel room)

    Material.foreground: Style.current.foreground
    Material.background: Style.current.background

    Settings {
        property alias userName: nameTextField.text
        property alias roomName: roomTextField.text
    }

    function setGuiEnabled(enabled) {
        guiEnabled = enabled
    }

    ColumnLayout {
        id: loginUi
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: historyUi.visible ? historyUi.left : parent.right
        spacing: 8
        enabled: guiEnabled

        Connections {
            target: joinModel
            onJoined: {
                joinPopup.close()
                joined(room)
            }
            onJoin_failed: {
                joinPopup.close()
                setGuiEnabled(true)
            }
        }

        NoVideo {
            id: noVideo
            Layout.minimumHeight: 0.4 * container.height
            Layout.minimumWidth: 0.5 * container.width
            Layout.maximumWidth: 600
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: false
            visible: (!ownMediaModel.videoAvailable
                      || ownMediaModel.videoDisabled) && !demoMode
            headerLabelText: getHeaderText()
            infoLabelText: getInfoText()

            AudioVideoOverlay {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 60
                videoOffButtonVisible: ownMediaModel.videoDisabled ? true : false
                settingsDialog: joinRoomContainer.settingsDialog
            }

            function getInfoText() {
                if (ownMediaModel.videoDisabled) {
                    var placeholderDevices = "camera"
                    var placeholderReference = "it"
                    if (ownMediaModel.muted) {
                        placeholderDevices = "camera and microphone"
                        placeholderReference = "them"
                    }

                    return qsTr("You have disabled your %1. After joining a room, you can re-enable %2 at any time using your action buttons at the lower left corner.").arg(
                                placeholderDevices).arg(placeholderReference)
                }

                return qsTr("Please check your video camera and settings and restart the app or continue without video...")
            }

            function getHeaderText() {
                if (ownMediaModel.videoDisabled) {
                    if (ownMediaModel.muted) {
                        return qsTr("Camera and microphone disabled")
                    }

                    return qsTr("Camera disabled")
                }

                return qsTr("No video camera found.")
            }
        }

        Loader {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: false
            sourceComponent: demoMode ? demoImageComponent : null
            Layout.minimumHeight: 0.4 * container.height
            Layout.minimumWidth: 0.5 * container.width
            Layout.maximumWidth: 600
            Layout.maximumHeight: 400
            visible: demoMode
        }

        Component {
            id: demoImageComponent

            Image {
                source: Style.current.demoImagesPath + "Sarah.jpg"
                fillMode: Image.PreserveAspectCrop

                AudioVideoOverlay {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 60
                    videoOffButtonVisible: true
                    settingsDialog: joinRoomContainer.settingsDialog
                }
            }
        }

        VideoOutput {
            id: videoOutput
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: false
            source: ownMediaModel.video
            visible: ownMediaModel.videoAvailable && !demoMode
                     && !ownMediaModel.videoDisabled

            function getAspectRatio() {
                return videoOutput.sourceRect.width / videoOutput.sourceRect.height
            }

            Layout.maximumWidth: {
                var percentage = 0.6
                var aspectRatio = getAspectRatio()
                return Math.min(container.width * percentage,
                                container.height * percentage * aspectRatio)
            }

            Layout.maximumHeight: {
                var percentage = 0.6
                var aspectRatio = getAspectRatio()
                return Math.min(container.height * percentage,
                                (container.width * percentage) / aspectRatio)
            }

            AudioVideoOverlay {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 60
                settingsDialog: joinRoomContainer.settingsDialog
                videoOffButtonVisible: true
            }
        }

        ColumnLayout {
            id: inputLayout
            Layout.fillWidth: false
            height: 60
            Layout.minimumWidth: 400
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            spacing: 15
            Layout.topMargin: 40

            Item {
                implicitHeight: roomTextField.height + 5
                Layout.fillWidth: true

                TextField {
                    id: roomTextField
                    leftPadding: 0
                    padding: 0
                    selectByMouse: true
                    placeholderText: qsTr("Room name *")
                    Layout.fillWidth: true
                    focus: true
                    anchors.right: parent.right
                    anchors.left: parent.left
                    onAccepted: nameTextField.focus = true

                    Settings {
                        property alias roomName: roomTextField.text
                    }
                }

                Label {
                    id: validRoomName
                    font.pointSize: Style.current.subTextPointSize
                    color: Style.current.accent
                    anchors.top: roomTextField.bottom
                    text: inputLayout.getRoomNameError(roomTextField.text)
                }
            }

            Item {
                implicitHeight: nameTextField.height + 5
                Layout.fillWidth: true

                TextField {
                    id: nameTextField
                    anchors.right: parent.right
                    anchors.left: parent.left
                    selectByMouse: true
                    placeholderText: qsTr("Your name *")
                    Layout.fillWidth: true
                    //onAccepted: passwordTextField.focus = true
                    onAccepted: {
                        if (joinButton.enabled) {
                            joinRoomContainer.joinRoom()
                        }
                    }
                }

                Label {
                    id: validName
                    color: Style.current.accent
                    font.pointSize: Style.current.subTextPointSize
                    anchors.top: nameTextField.bottom
                    text: inputLayout.getNameError(nameTextField.text)
                }
            }

            //            Item {
            //                implicitHeight: passwordTextField.height + 20
            //                Layout.fillWidth: true

            //                TextField {
            //                    id: passwordTextField
            //                    anchors.right: parent.right
            //                    anchors.left: parent.left
            //                    selectByMouse: true
            //                    placeholderText: qsTr("Password (optional)")
            //                    Layout.fillWidth: true
            //                    onAccepted: joinRoomContainer.joinRoom()

            //                    onTextChanged: {
            //                        validPassword.visible = false
            //                    }
            //                }

            //                Label {
            //                    id: validPassword
            //                    color: Style.current.accent
            //                    font.pointSize: Style.current.subTextPointSize
            //                    visible: false
            //                    anchors.top: passwordTextField.bottom
            //                }
            //            }
            Button {
                id: joinButton
                width: 300
                text: getJoinButtonText()
                Layout.fillHeight: false
                Material.background: Style.current.primary
                Material.foreground: Style.current.buttonTextColor
                Layout.preferredWidth: 300
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: false
                onClicked: joinRoomContainer.joinRoom()
                enabled: inputLayout.getRoomNameError() === ""
                         && inputLayout.getNameError() === ""

                function getJoinButtonText() {
                    if (!ownMediaModel.videoAvailable
                            || ownMediaModel.videoDisabled) {
                        if (ownMediaModel.muted) {
                            return qsTr("Join without video and audio")
                        }

                        return qsTr("Join without video")
                    } else if (ownMediaModel.muted) {
                        return qsTr("Join without audio")
                    }

                    return qsTr("Join")
                }
            }

            function getRoomNameError() {
                if (roomTextField.text.length === 0) {
                    return qsTr("Please enter a room name.")
                } else if (roomTextField.text.length > 50) {
                    return qsTr("Room name is too long (max 50 characters).")
                }

                return ""
            }

            function getNameError() {
                if (nameTextField.text.length === 0) {
                    return qsTr("Please enter a name.")
                } else if (nameTextField.text.length > 50) {
                    return qsTr("Name is too long (max 50 characters).")
                }

                return ""
            }
        }
    }

    CustomRectangle {
        id: historyUi
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        lBorderwidth: 1
        color: Style.current.background
        borderColor: Style.current.foreground
        width: 300
        visible: history.hasRoomHistory
                 && settingsDialog.generalSettings.showRoomHistory

        RoomHistory {
            id: history
            anchors.fill: parent
            anchors.topMargin: 20
            anchors.bottomMargin: 20
            anchors.leftMargin: 20
            onJoinRoom: {
                roomTextField.text = roomName
                joinRoomContainer.joinRoom()
            }
        }
    }

    ProgressPopup {
        id: joinPopup
        progressText: qsTr("Joining the room...")
        isJoinPopup: true
    }

    function joinRoom() {
        joinPopup.open()

        //        if (passwordTextField.text) {
        //            joinModel.joinWithPassword(roomTextField.text, nameTextField.text,
        //                                       passwordTextField.text)
        //        } else {

        //        }
        joinModel.join(roomTextField.text, nameTextField.text)

        guiEnabled = false
    }

    function isEmpty(text) {
        return text.length === 0
    }

    function isTooLong(text) {
        return text.length > 50
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

