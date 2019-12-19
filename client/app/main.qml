import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.0
import io.fubble.FrameProvider 1.0

Window {
  visible: true
  width: 640
  height: 480
  title: qsTr("Hello World")

  FrameProvider {
    objectName: "provider"
    id: provider
  }

  VideoOutput {
    id: display
    objectName: "display"
    anchors.fill: parent
    source: provider
  }

  /*
  ClientForm {
    anchors.fill: parent
  }
  */
}


