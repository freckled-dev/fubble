import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtMultimedia 5.0
import io.fubble 1.0
import Qt.labs.settings 1.0 as QtSettings
import QtQuick.Controls.Material 2.0
import "."

FocusScope {
    property var title: qsTr("Join a room")
    property JoinModel joinModel
    signal joined(RoomModel room)
    Material.foreground: Style.current.foreground
    property bool guiEnabled: true

    QtSettings.Settings {
        property alias userName: nameTextField.text
        property alias roomName: roomTextField.text
    }

    function setGuiEnabled(enabled) {
        guiEnabled = enabled
    }

    ColumnLayout {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        id: loginUi
        spacing: 8
        enabled: guiEnabled

        Connections {
            target: joinModel
            onJoined: {
                joined(room)
            }
            onJoin_failed: {
                setGuiEnabled(true)
            }
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

            joinModel.join(roomTextField.text, nameTextField.text)
            guiEnabled = false
        }

        function isEmpty(text) {
            return text.length === 0
        }

        NoVideo {
            id: noVideo
            width: 500
            height: 300
            visible: !joinModel.videoAvailable
        }

        VideoOutput {
            id: videoOutput
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
            width: 500
            height: 100
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
                    onAccepted: loginUi.joinRoom()

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
                Layout.topMargin: 20
                Layout.fillHeight: false
                Material.background: Style.current.primary
                Material.foreground: Style.current.buttonTextColor
                Layout.preferredWidth: 300
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: false
                onClicked: loginUi.joinRoom()
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:4;anchors_width:500;anchors_x:0;anchors_y:"-248"}
}
##^##*/

