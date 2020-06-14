import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import io.fubble 1.0
import "."

Item {
    id: participantContainer
    Layout.fillWidth: true
    implicitHeight: participantColumn.height

    ColumnLayout {
        id: participantColumn
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.leftMargin: 0
        implicitHeight: overview.height

        Item {
            id: overview
            Layout.fillWidth: true
            implicitHeight: nameLabel.height + 10

            Rectangle {
                id: participantBorder
                anchors.fill: parent
                color: model.participant.highlighted
                       || mouseArea.containsMouse ? Style.current.gray300 : Style.current.gray100
                radius: 5
                border.color: model.participant.voiceDetected ? Style.current.primary : Style.current.transparent
                visible: true
            }

            Label {
                id: nameLabel
                anchors.left: parent.left
                anchors.leftMargin: 10
                color: Style.current.foreground
                text: model.participant.name
                anchors.verticalCenter: parent.verticalCenter
            }

            Image {
                id: videoDisabledImage
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: mutedImage.left
                sourceSize.height: 20
                sourceSize.width: 20
                source: Style.current.videoDisabledImage
                visible: model.participant.videoDisabled
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    details.visible = !details.visible
                    audioChart.visible = !audioChart.visible
                }
            }

            Image {
                id: mutedImage
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                sourceSize.height: 20
                sourceSize.width: 20
                source: model.participant.silenced ? Style.current.silencedImage : Style.current.mutedImage
                visible: model.participant.muted || model.participant.silenced

                FubbleToolTip {
                    id: ttMuted
                    text: model.participant.silenced ? qsTr("Muted by you") : qsTr(
                                                           "Muted")
                    visible: maMuted.containsMouse
                }

                MouseArea {
                    id: maMuted
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        AudioChart {
            id: audioChart
            audioParticipant: participant
            Layout.fillWidth: true
            visible: false
            height: 100
        }

        Item {
            id: details
            visible: false
            Layout.fillWidth: true
            implicitHeight: 70

            Label {
                id: volumeHeader
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                text: qsTr("Volume")
            }

            VolumeSlider {
                id: volumeSlider
                sliderColor: volumeSlider.enabled ? Style.current.primary : Style.current.gray300
                value: model.participant.volume
                anchors.top: volumeHeader.bottom
                anchors.left: parent.left
                anchors.right: muteImage.left
                enabled: !participant.muted
            }

            Image {
                id: muteImage
                anchors.verticalCenter: volumeSlider.verticalCenter
                sourceSize.height: 20
                sourceSize.width: 20
                anchors.right: parent.right
                source: model.participant.muted ? Style.current.mutedImage : Style.current.mutedOffImage

                MouseArea {
                    id: maMute
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        participant.silenced = !participant.silenced
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

