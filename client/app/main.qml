import QtMultimedia 5.0
import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
import io.fubble.FrameProvider 1.0

Window {
  visible: true
  minimumWidth: 640
  minimumHeight: 480
  title: qsTr("fubble")
  id: container

  StackView {
    id: stack
    initialItem: login
    anchors.fill: parent
    focus: true
  }

    Component {
      id: roomComponent
      GridLayout {
        VideoOutput {
          source: videosModel.video
          Layout.maximumWidth: 500
          Layout.maximumHeight: 500
        }
        /*
        VideoOutput {
          source: provider
          // Layout.fillWidth: true; Layout.fillHeight: true
          Layout.maximumWidth: 500
          Layout.maximumHeight: 500
        }
        */
        Button {
          text: "button"
        }
      }
    }

    Component {
      id: login
      FocusScope {
        ColumnLayout {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            id: loginUi
            function joinRoom() {
                joinModel.join(room.text, name.text)
                stack.push(roomComponent)
            }

            VideoOutput {
                id: videoOutput
                source: provider
                function getAspectRatio() {
                  return videoOutput.sourceRect.width / videoOutput.sourceRect.height
                }
                Layout.maximumWidth: {
                  var percentage = 0.6
                  var aspectRatio = getAspectRatio()
                  return Math.min(container.width*percentage, container.height*percentage*aspectRatio)
                }
                Layout.maximumHeight: {
                  var percentage = 0.6
                  var aspectRatio = getAspectRatio()
                  return Math.min(container.height*percentage, (container.width*percentage)/aspectRatio)
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

  FrameProvider {
    objectName: "provider"
    id: provider
  }
}


