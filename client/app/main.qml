import QtMultimedia 5.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import io.fubble 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 640
    minimumHeight: 480
    title: "fubble"
    id: container

    Action {
        id: optionsMenuAction
        // icon.name: "icon should be part of theme!"
        icon.source: "cog.svg"
        onTriggered: optionsMenu.open()
    }

    header: ToolBar {
        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
            // action: navigateBackAction
            }

            Label {
                id: titleLabel
                // text: listView.currentItem ? listView.currentItem.text : "Join a room"
                text: stack.currentItem.title
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                action: optionsMenuAction
                // text: qsTr("‹")

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

                    Action {
                        text: "About"
                        onTriggered: aboutDialog.open()
                    }
                }
            }
        }
    }

    StackView {
        id: stack
        initialItem: login
        anchors.fill: parent
        focus: true
    }

    Component {
        id: roomComponent
        GridLayout {
            id: layout
            property RoomModel room
            property var title: layout.room.name

            /*
        VideoOutput {
          source: videosModel.video
          Layout.fillWidth: true; Layout.fillHeight: true
        }
        VideoOutput {
          source: videosModel.ownVideo
          Layout.fillWidth: true; Layout.fillHeight: true
        }
        */
        }
    }

    Component {
        id: login
        FocusScope {
          property var title: "Join a room"
            ColumnLayout {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                id: loginUi

                Connections {
                    target: joinModel
                    onJoined: {
                        stack.push(roomComponent, { "room": room })
                    }
                }
                function joinRoom() {
                    joinModel.join(room.text, name.text)
                    loginUi.enabled = false
                }
                VideoOutput {
                    id: videoOutput
                    source: videosModel.ownVideo
                    function getAspectRatio() {
                        return videoOutput.sourceRect.width / videoOutput.sourceRect.height
                    }
                    Layout.maximumWidth: {
                        var percentage = 0.6
                        var aspectRatio = getAspectRatio()
                        return Math.min(
                                    container.width * percentage,
                                    container.height * percentage * aspectRatio)
                    }
                    Layout.maximumHeight: {
                        var percentage = 0.6
                        var aspectRatio = getAspectRatio()
                        return Math.min(
                                    container.height * percentage,
                                    (container.width * percentage) / aspectRatio)
                    }
                }
                TextField {
                    id: room
                    text: joinModel.room
                    placeholderText: "Room"
                    Layout.fillWidth: true
                    focus: true
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
                    text: qsTr("Join")
                    Layout.fillWidth: true
                    onClicked: loginUi.joinRoom()
                }
            }
        }
      }
      Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: "About"
        x: (container.width - width) / 2
        y: container.height / 6
        width: Math.min(container.width, container.height) / 3 * 2
        contentHeight: aboutColumn.height

        Column {
            id: aboutColumn
            spacing: 20

            Label {
                width: aboutDialog.availableWidth
                text: "Fubble by <i>Freckled OG</i>.<br><a href='https://freckled.dev/contact'>Contact us at freckled.dev/contact</a>"
                onLinkActivated: Qt.openUrlExternally(link)
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
    }   }
}
