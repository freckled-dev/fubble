import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

Rectangle {
    id: videoWallContainer
    property RoomModel roomModel
    property alias videoCount: participantRepeater.count
    color: Style.current.background
    visible: videoCount !== 0

    GridLayout {
        id: participantGrid
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        rowSpacing: 10
        columnSpacing: 10
        columns: videoWallContainer.calculateColumns()

        Repeater {
            id: participantRepeater
            model: roomModel.participantsWithVideo
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
    }

    // Chat
    function calculateColumns() {
        var aspectRatio = participantGrid.width / participantGrid.height
        var numParticipants = participantRepeater.count
        // aspectRatio = aspectRatio * Math.sqrt(numParticipants)
        aspectRatio = Math.round(aspectRatio)
        aspectRatio = Math.max(1, aspectRatio)
        return aspectRatio
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

