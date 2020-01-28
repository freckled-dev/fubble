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


    header: Header {
      title: stack.currentItem.title
    }
    StackView {
        id: stack
        initialItem: Join {}
        anchors.fill: parent
        focus: true
    }

    Component {
        id: roomComponent
        GridLayout {
            id: layout
            columns: 2
            property RoomModel room
            property var title: layout.room.name

            Repeater {
              model: layout.room.participants
                 Participant {
                  participant: model.participant
              }
            }

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

}
