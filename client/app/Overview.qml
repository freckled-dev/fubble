import QtMultimedia 5.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import QtQml.Models 2.12
import io.fubble 1.0
import "."

Item {
    id: overviewContainer
    property RoomModel roomModel
    property bool overviewVisible: true

    onOverviewVisibleChanged: {
        if (overviewVisible) {
            roomModel.resetNewParticipants()
        }
    }

    Item {
        anchors.fill: parent
        anchors.margins: 10

        Label {
            id: participantLabel
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            text: qsTr("Participants")
            visible: overviewVisible
            font.pointSize: Style.current.subHeaderPointSize
        }

        ListView {
            model: participantColumns
            anchors.top: participantLabel.bottom
            anchors.right: parent.right
            anchors.bottom: selfAction.top
            anchors.left: parent.left
            anchors.topMargin: 30
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
            visible: overviewVisible
            anchors.bottom: parent.bottom
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

