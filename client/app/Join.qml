import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtMultimedia 5.0
import io.fubble 1.0
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.0
import "."

FocusScope {
    id: joinRoomContainer
    property var title: qsTr("Join a room")
    property JoinModel joinModel
    property bool guiEnabled: true
    property alias history: history
    property alias roomName: roomTextField.text

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
                setGuiEnabled(true)
            }
        }

        NoVideo {
            id: noVideo
            width: 500
            height: 300
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: false
            visible: !joinModel.videoAvailable
        }

        VideoOutput {
            id: videoOutput
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: false
            source: joinModel.video
            visible: joinModel.videoAvailable

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
        }

        ColumnLayout {
            id: inputLayout
            Layout.fillWidth: false
            height: 60
            Layout.minimumWidth: 500
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            spacing: 8
            Layout.topMargin: 40

            Item {
                implicitHeight: roomTextField.height + roomMandatory.height
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

                    onTextChanged: {
                        roomMandatory.visible = false
                    }
                }

                Label {
                    id: roomMandatory
                    text: qsTr("Please enter a room name.")
                    font.pointSize: Style.current.subTextPointSize
                    color: Style.current.accent
                    anchors.top: roomTextField.bottom
                    visible: false
                }
            }

            Item {
                implicitHeight: nameTextField.height + nameTextField.height
                Layout.fillWidth: true

                TextField {
                    id: nameTextField
                    anchors.right: parent.right
                    anchors.left: parent.left
                    selectByMouse: true
                    placeholderText: qsTr("Your name *")
                    Layout.fillWidth: true
                    onAccepted: joinRoomContainer.joinRoom()

                    onTextChanged: {
                        nameMandatory.visible = false
                    }
                }

                Label {
                    id: nameMandatory
                    color: Style.current.accent
                    font.pointSize: Style.current.subTextPointSize
                    text: qsTr("Please enter a (nick) name.")
                    visible: false
                    anchors.top: nameTextField.bottom
                }
            }

            Button {
                id: joinButton
                width: 300
                text: qsTr("Join")
                Layout.fillHeight: false
                Material.background: Style.current.primary
                Material.foreground: Style.current.buttonTextColor
                Layout.preferredWidth: 300
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: false
                onClicked: joinRoomContainer.joinRoom()
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
        visible: history.hasRoomHistory && header.settings.showRoomHistory

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
        var noRoomName = isEmpty(roomTextField.text)
        var noName = isEmpty(nameTextField.text)

        if (noRoomName) {
            roomMandatory.visible = true
        }

        if (noName) {
            nameMandatory.visible = true
        }

        if (noRoomName || noName) {
            return
        }

        joinPopup.open()

        joinModel.join(roomTextField.text, nameTextField.text)
        guiEnabled = false
    }

    function isEmpty(text) {
        return text.length === 0
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

