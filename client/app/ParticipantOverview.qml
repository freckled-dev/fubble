import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import io.fubble 1.0
import "."

Rectangle {
    id: participantContainer
    color: Style.current.transparent
    implicitWidth: nameLabel.implicitWidth
    implicitHeight: nameLabel.implicitHeight

    Item {
        id: element
        anchors.fill: parent

        Label {
            id: nameLabel
            anchors.left: parent.left
            color: Style.current.foreground
            padding: 8
            text: model.participant.name
            anchors.verticalCenter: parent.verticalCenter
        }

        Image {
            id: videoDisabledImage
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: mutedImage.left
            sourceSize.height: 20
            sourceSize.width: 20
            anchors.rightMargin: 8
            source: "pics/video_disabled.svg"
            visible: model.participant.videoDisabled
        }

        Image {
            id: mutedImage
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            sourceSize.height: 20
            sourceSize.width: 20
            anchors.rightMargin: 8
            source: "pics/muted.svg"
            visible: model.participant.muted
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            //participantContainer.color = Style.current.primary
            participantContainer.radius = 5
            participantContainer.border.width = 1
            //            nameLabel.color = Qt.binding(function () {
            //                return Style.current.buttonTextColor
            //            })
        }
        onExited: {
            //participantContainer.color = Style.current.transparent
            participantContainer.radius = 0
            participantContainer.border.width = 0
            //            nameLabel.color = Qt.binding(function () {
            //                return Style.current.foreground
            //            })
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

