import QtMultimedia 5.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import io.fubble 1.0

GridLayout {
  property ParticipantModel participant
  columns: 1
  ToolBar {
    Layout.fillWidth: true
    RowLayout {
      anchors.fill: parent
      Label {
        Layout.fillWidth: true
        font.pixelSize: 20
        text: participant.name
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
      }
    }
  }
  VideoOutput {
    // source: videosModel.ownVideo
    source: participant.video
    Layout.fillWidth: true
    Layout.fillHeight: true
  }
}