import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.0
import io.fubble.FrameProvider 1.0

Window {
  visible: true
  minimumWidth: 640
  minimumHeight: 480
  title: qsTr("fubble")

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


