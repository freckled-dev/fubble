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

  ColumnLayout {
      id: columnLayout
      anchors.verticalCenter: parent.verticalCenter
      anchors.horizontalCenter: parent.horizontalCenter

      VideoOutput {
          id: videoOutput
          source: provider
          Layout.maximumWidth: {
            var percentage = 0.6
            var aspectRatio = videoOutput.sourceRect.width / videoOutput.sourceRect.height
            return Math.min(container.width*percentage, container.height*percentage*aspectRatio)
          }
          Layout.maximumHeight: {
            var percentage = 0.6
            var aspectRatio = videoOutput.sourceRect.width / videoOutput.sourceRect.height
            return Math.min(container.height*percentage, (container.width*percentage)/aspectRatio)
          }
      }
      TextField {
          id: room
          text: backend.room
          placeholderText: "Room"
          Layout.fillWidth: true
          focus: true
          onAccepted: name.focus = true
      }
      TextField {
          id: name
          text: backend.name
          placeholderText: "Your Name"
          Layout.fillWidth: true
          onAccepted: backend.join(room.text, name.text)
      }
      Button {
          id: join
          text: qsTr("Join")
          Layout.fillWidth: true
          onClicked: backend.join(room.text, name.text)
      }
  }

  FrameProvider {
    objectName: "provider"
    id: provider
  }
}


