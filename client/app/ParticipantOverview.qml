import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
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
        spacing: 0
        anchors.leftMargin: 0
        implicitHeight: overview.height

        Item {
            id: overview
            Layout.fillWidth: true
            implicitHeight: nameLabel.height + 10

            Rectangle {
                id: participantBorder
                anchors.fill: parent
                color: participant.highlighted
                       || maHeader.containsMouse ? Style.current.gray300 : Style.current.gray100
                radius: 5
                border.color: participant.voiceDetected ? Style.current.primary : Style.current.transparent
                visible: true
            }

            Label {
                id: nameLabel
                anchors.left: parent.left
                anchors.leftMargin: 10
                color: Style.current.foreground
                text: participant.name
                anchors.verticalCenter: parent.verticalCenter
            }

            MouseArea {
                id: maHeader
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    details.visible = !details.visible
                }
            }

            Image {
                id: mutedImage
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: deafedImage.visible ? deafedImage.left : parent.right
                sourceSize.height: 20
                sourceSize.width: 20
                anchors.rightMargin: 5
                source: participant.silenced ? Style.current.silencedImage : Style.current.mutedImage
                visible: participant.muted || participant.silenced || participant.deafed

                FubbleToolTip {
                    id: ttMuted
                    text: participant.silenced ? qsTr("Muted by you") : qsTr(
                                                     "Muted")
                    visible: maMuted.containsMouse
                }

                MouseArea {
                    id: maMuted
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            Image {
                id: deafedImage
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                sourceSize.height: 20
                sourceSize.width: 20
                anchors.rightMargin: 5
                source: Style.current.deafedImage
                visible: participant.deafed

                FubbleToolTip {
                    id: ttDeafed
                    text: qsTr("User has deafed himself.")
                    visible: maDeafed.containsMouse
                }

                MouseArea {
                    id: maDeafed
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }

        Rectangle {
            id: details
            border.color: participant.highlighted
                          || maHeader.containsMouse ? Style.current.gray300 : Style.current.gray100
            Layout.fillWidth: true
            color: Style.current.transparent
            visible: false
            radius: 5
            implicitHeight: audioChart.height + 10 + moreDetails.height

            Item {
                id: moreDetails

                anchors.left: parent.left
                anchors.right: parent.right
                height: participant.own ? 0 : 70
                visible: !participant.own

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
                    // leads to warning about loop detected. else works like a charm
                    // value: participant.volume
                    value: 1
                    onValueChanged: participant.volume = value
                    anchors.top: volumeHeader.bottom
                    anchors.left: parent.left
                    anchors.right: muteImage.left
                    anchors.rightMargin: 5
                    enabled: !participant.silenced
                }

                FubbleActionButton {
                    id: muteImage
                    anchors.verticalCenter: volumeSlider.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    showToolTip: false
                    icon.source: participant.silenced ? Style.current.mutedImage : Style.current.mutedOffImage
                    onActionClick: participant.silenced = !participant.silenced
                }
            }

            Connections {
                target: participant
                onNewAudioLevel: {
                    audioChart.addNewAudioLevel(level)
                }
            }

            AudioChart {
                id: audioChart
                anchors.top: moreDetails.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                height: 60
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

