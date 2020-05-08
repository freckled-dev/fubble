import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import io.fubble 1.0
import "."

Rectangle {
    id: participantContainer
    property ParticipantModel participant
    color: Style.current.transparent
    implicitWidth: nameLabel.implicitWidth
    implicitHeight: nameLabel.implicitHeight

    Label {
        id: nameLabel
        color: Style.current.foreground
        font.pointSize: Style.current.normalPointSize
        padding: 8
        text: model.participant.name
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            participantContainer.color = Style.current.primary
            participantContainer.radius = 5
            participantContainer.border.width = 1
            nameLabel.color = Qt.binding(function () {
                return Style.current.buttonTextColor
            })
        }
        onExited: {
            participantContainer.color = Style.current.transparent
            participantContainer.radius = 0
            participantContainer.border.width = 0
            nameLabel.color = Qt.binding(function () {
                return Style.current.foreground
            })
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

