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
        Rectangle {
            id: noImageBorder
            width: 500
            height: 300
            color: "#00000000"
            radius: 5
            border.color: Style.current.foreground
            border.width: 1
            Material.foreground: Style.current.foreground
            Layout.fillWidth: true

            Flipable {
                id: noImageFlipable
                visible: !joinModel.videoAvailable
                property bool flipped: false
                anchors.rightMargin: 40
                anchors.leftMargin: 40
                anchors.fill: parent

                front: ColumnLayout {
                    id: noImageColumnLayout
                    spacing: 20
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.left: parent.left

                    Label {
                        text: qsTr("No video camera found.")
                        font.pointSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }
                    Label {
                        text: qsTr("Please check your video camera and settings and restart the app or continue without video...")
                        font.pointSize: 12
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }
                }
                back: Image {
                    anchors.centerIn: parent
                    height: 240
                    width: 240
                    source: "pics/no_video.svg"
                }

                transform: Rotation {
                    id: rotation
                    origin.x: noImageFlipable.width / 2
                    origin.y: noImageFlipable.height / 2
                    axis.x: 0
                    axis.y: 1
                    axis.z: 0 // set axis.y to 1 to rotate around y-axis
                    angle: 0 // the default angle
                }

                states: State {
                    name: "back"
                    PropertyChanges {
                        target: rotation
                        angle: 180
                    }
                    when: noImageFlipable.flipped
                }

                transitions: Transition {
                    NumberAnimation {
                        target: rotation
                        property: "angle"
                        duration: 1000
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: noImageFlipable.flipped = !noImageFlipable.flipped
                }
            }
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
                placeholderText: qsTr("Room")
                Layout.fillWidth: true
                focus: true
                onAccepted: name.focus = true
            }

            TextField {
                id: name
                text: joinModel.name
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

