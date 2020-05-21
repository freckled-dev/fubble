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

    VideoLayout {
        id: participantGrid
        anchors.fill: parent
        visible: videoCount !== 0

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
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

