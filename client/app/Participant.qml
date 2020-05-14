import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0
import "."

Item {
    // layer.enabled: true
    // border.color: Style.current.foreground
    property ParticipantModel participant

    VideoOutput {
        id: video
        anchors.fill: parent
        source: participant.video
        fillMode: VideoOutput.Stretch
        visible: participant.video !== null

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                overlay.visible = true
                overlayBorder.visible = true
            }
            onExited: {
                overlay.visible = false
                overlayBorder.visible = false
            }
        }

        Rectangle {
            id: overlay
            color: Style.current.gray300Transparent
            anchors.left: parent.left
            anchors.right: parent.right
            visible: false
            implicitHeight: nameLabel.height
            anchors.bottom: video.bottom

            Label {
                id: nameLabel
                text: participant.name
                padding: 10
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Rectangle {
            id: overlayBorder
            border.color: Style.current.foreground
            color: Style.current.transparent
            radius: 2
            border.width: 1
            anchors.fill: parent
            visible: false
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

