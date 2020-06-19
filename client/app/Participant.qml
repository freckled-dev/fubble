import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0
import "."

Rectangle {
    property ParticipantModel participant
    property double aspect: {
        var width = video.sourceRect.width
        var height = video.sourceRect.height
        var result = width / height
        return result
    }

    VideoOutput {
        id: video
        anchors.fill: parent
        source: participant.video
        visible: participant.video !== null
        fillMode: VideoOutput.Stretch

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                participant.highlighted = Qt.binding(function () {
                    return true
                })
            }
            onExited: {
                participant.highlighted = Qt.binding(function () {
                    return false
                })
            }
        }

        ParticipantOverlay {
            id: overlay
            overlayParticipant: participant
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

