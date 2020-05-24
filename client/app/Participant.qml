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
        // console.log("width:" + width + ", height:" + height + ", result:" + result)
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

        Rectangle {
            id: overlay
            color: Style.current.gray300Transparent
            anchors.left: parent.left
            anchors.right: parent.right
            visible: participant.highlighted
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
            visible: participant.highlighted
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

