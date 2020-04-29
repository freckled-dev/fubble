import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0

Rectangle {
    id: videoWall
    color: "#236ab1"

    ColumnLayout {
        height: 21
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.left: parent.left

        Label {
            text: qsTr("Participants")
            font.pointSize: 14
            Layout.margins: 20
            Layout.alignment: Qt.AlignHCenter
            height: 40
        }

        GridLayout {
            id: participantGrid
            Layout.fillWidth: true
            rowSpacing: 10
            columnSpacing: 10
            columns: 4

            Repeater {
                model: layout.room.participants
                delegate: participantComponent
            }

            Component {
                id: participantComponent

                Participant {
                    Layout.fillWidth: true
                    participant: model.participant
                    Layout.preferredHeight: width
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;anchors_height:21;anchors_width:640}
}
##^##*/

