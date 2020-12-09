import QtMultimedia 5.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import io.fubble 1.0
import "."

Item {
    id: overviewContainer
    property RoomModel roomModel
    property FubbleActionButton overviewShowIcon

    onVisibleChanged: {
        if (visible) {
            roomModel.resetNewParticipants()
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: 10

        Item {
            id: participantHeader
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            implicitHeight: participantLabel.implicitHeight

            Label {
                id: participantLabel
                text: qsTr("Participants")
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.left: parent.left
                anchors.right: collapseButton.left
                font.pointSize: Style.current.subHeaderPointSize
            }

            FubbleActionButton {
                id: collapseButton
                anchors.verticalCenter: parent.verticalCenter
                icon.source: Style.current.collapseImageLeft
                toolTipText: qsTr("Hide participant view")
                anchors.right: parent.right
                buttonWidth: 25
                buttonHeight: 35
                onActionClick: {
                    overviewContainer.visible = false
                    overviewShowIcon.visible = true
                }
            }
        }

        ListView {
            model: participantColumns
            anchors.top: participantHeader.bottom
            anchors.right: parent.right
            anchors.bottom: selfAction.top
            anchors.left: parent.left
            anchors.topMargin: 15
            anchors.bottomMargin: 30
            clip: true
            spacing: 5
        }

        SortFilterModel {
            id: participantColumns
            model: overviewContainer.roomModel.participants
            delegate: participantOverviewComponent
            lessThan: function (left, right) {
                var leftParticipant = left.participant
                var rightParticipant = right.participant

                // own participant comes first
                if (leftParticipant.own) {
                    return true
                }
                if (rightParticipant.own) {
                    return false
                }

                // sort by name
                return leftParticipant.name < rightParticipant.name
            }
        }

        Component {
            id: participantOverviewComponent
            ParticipantOverview {
                anchors.left: parent.left
                anchors.right: parent.right
            }
        }

        ParticipantAction {
            id: selfAction
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

