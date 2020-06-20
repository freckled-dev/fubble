import QtQuick 2.0
import QtQuick.Controls 2.2
import io.fubble 1.0
import "."

Item {
    anchors.fill: parent

    property ParticipantModel overlayParticipant

    Rectangle {
        id: overlay
        color: Style.current.gray300Transparent
        anchors.left: parent.left
        anchors.right: parent.right
        visible: overlayParticipant.highlighted
        implicitHeight: nameLabel.height
        anchors.bottom: parent.bottom

        Label {
            id: nameLabel
            text: overlayParticipant.name
            padding: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Rectangle {
        id: overlayBorder
        border.color: model.participant.voiceDetected ? Style.current.primary : Style.current.foreground
        color: Style.current.transparent
        radius: 2
        border.width: 1
        anchors.fill: parent
        visible: overlayParticipant.highlighted
                 || model.participant.voiceDetected
    }
}
