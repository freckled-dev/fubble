import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."
import "scripts/utils.js" as Utils

Rectangle {
    id: videoWallContainer
    property RoomModel roomModel
    color: Style.current.background
    visible: roomModel.videosAvailable
    property bool demoMode: Utils.isDemoMode()

    VideoLayout {
        id: participantGrid
        anchors.fill: parent

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
                demoMode: videoWallContainer.demoMode
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

