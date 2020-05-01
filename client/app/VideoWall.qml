import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0

Rectangle {
    id: videoWall
    color: Style.current.background

    ColumnLayout {
        anchors.margins: 20
        anchors.fill: parent

        Label {
            text: qsTr("Video Wall")
            Layout.bottomMargin: 20
            font.pointSize: 14
            Layout.alignment: Qt.AlignHCenter
            height: 40
        }

        GridLayout {
            id: participantGrid
            Layout.fillWidth: true
            rowSpacing: 10
            columnSpacing: 10
            columns: calculateColumns()

            Repeater {
                model: layout.room.participantsWithVideo
                delegate: participantComponent
            }

            Component {
                id: participantComponent

                Participant {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    participant: model.participant
                }
            }

            function calculateColumns() {
                var aspectRatio = participantGrid.width / participantGrid.height
                var numParticipants = layout.room.participants.length
                aspectRatio = aspectRatio * Math.sqrt(numParticipants)
                aspectRatio = Math.round(aspectRatio)
                aspectRatio = Math.max(1, aspectRatio)
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

