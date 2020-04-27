import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtMultimedia 5.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

FocusScope {
    property var title: "Join a room"
    property JoinModel joinModel
    signal joined(RoomModel room)
    Material.foreground: Style.current.foreground

    ColumnLayout {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        id: loginUi
        spacing: 8

        Connections {
            target: joinModel
            onJoined: {
                joined(room)
            }
        }
        function joinRoom() {
            joinModel.join(room.text, name.text)
            loginUi.enabled = false
        }
        VideoOutput {
            id: videoOutput
            source: joinModel.video
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
            width: 100
            height: 100
            spacing: 8
            Layout.topMargin: 40

            TextField {
                id: room
                text: joinModel.room
                placeholderText: "Room"
                Layout.fillWidth: true
                focus: true
                placeholderTextColor: Style.current.foreground
                color: Style.current.foreground
                onAccepted: name.focus = true
            }

            TextField {
                id: name
                text: joinModel.name
                placeholderText: "Your Name"
                Layout.fillWidth: true
                onAccepted: loginUi.joinRoom()
            }

            Button {
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
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

