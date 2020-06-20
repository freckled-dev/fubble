import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0
import "."
import "scripts/utils.js" as Utils

Rectangle {
    property ParticipantModel participant
    property bool demoMode: false

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
        visible: participant.video !== null && !demoMode
        fillMode: VideoOutput.Stretch

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: highlightParticipant(true)
            onExited: highlightParticipant(false)
        }

        ParticipantOverlay {
            overlayParticipant: participant
        }
    }

    Image {
        visible: demoMode
        anchors.fill: parent
        source: Style.current.demoImagesPath + participant.name + ".jpg"
        fillMode: Image.PreserveAspectCrop

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: highlightParticipant(true)
            onExited: highlightParticipant(false)
        }

        ParticipantOverlay {
            overlayParticipant: participant
        }
    }

    function highlightParticipant(highlight) {
        participant.highlighted = Qt.binding(function () {
            return highlight
        })
    }
}
