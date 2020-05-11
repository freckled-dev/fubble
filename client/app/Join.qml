import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtMultimedia 5.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

FocusScope {
    property var title: qsTr("Join a room")
    property JoinModel joinModel
    signal joined(RoomModel room)
    Material.foreground: Style.current.foreground
    property bool guiEnabled: true

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
        }
        function joinRoom() {
            joinModel.join(room.text, name.text)
            guiEnabled = false
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
            width: 100
            height: 100
            spacing: 8
            Layout.topMargin: 40

            TextField {
                id: room
                text: joinModel.room
                leftPadding: 0
                padding: 0
                placeholderText: qsTr("Room")
                Layout.fillWidth: true
                focus: true
                onAccepted: name.focus = true
            }

            TextField {
                id: name
                text: joinModel.name
                leftPadding: 0
                padding: 0
                placeholderText: qsTr("Your Name")
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
    D{i:0;autoSize:true;height:480;width:640}D{i:4;anchors_width:500;anchors_x:0;anchors_y:"-248"}
}
##^##*/

