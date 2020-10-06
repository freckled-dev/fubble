import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.12
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
                onEnlarge: participantGrid.enlarge(this)
                onShrink: participantGrid.shrink(this)
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

