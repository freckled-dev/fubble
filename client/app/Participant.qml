import QtMultimedia 5.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.14
import QtGraphicalEffects 1.12
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
    signal enlarge()
    signal shrink()

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
            // https://doc.qt.io/qt-5/qml-qtquick-mousearea.html#acceptedButtons-prop
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                if (mouse.button == Qt.LeftButton)
                    enlarge()
                if (mouse.button == Qt.RightButton)
                    shrink()
            }
        }

        ParticipantOverlay {
            overlayParticipant: participant
        }
    }

    Loader {
        sourceComponent: demoMode ? demoImageComponent : null
        visible: demoMode
        anchors.fill: parent
    }

    Component {
        id: demoImageComponent

        Image {
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
    }

    function highlightParticipant(highlight) {
        participant.highlighted = Qt.binding(function () {
            return highlight
        })
    }
}
