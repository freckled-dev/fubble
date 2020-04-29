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
        anchors.fill: parent

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
            columns: 2

            Repeater {
                model: layout.room.participants
                delegate: participantComponent
            }

            Component {
                id: participantComponent

                Participant {
                    //Layout.preferredWidth: participantGrid.width / participantGrid.columns - 20
                    //Layout.preferredHeight: participantGrid.height / participantGrid.rows - 20
                    //Layout.fillHeight: true
                    participant: model.participant
                }
            }

            function calculateColumns() {
                var aspectRatio = Math.round(
                            participantGrid.width / participantGrid.height)
                aspectRatio = Math.max(1, aspectRatio)
                console.log(aspectRatio)
                return aspectRatio
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;anchors_height:21;anchors_width:640}
}
##^##*/

