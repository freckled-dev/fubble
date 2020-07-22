import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import io.fubble 1.0
import "."

Item {
    id: overviewContainer
    property RoomModel roomModel
    property int overviewWidth: 250
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
            visible: overviewVisible || overviewAnimation.running
            font.pointSize: Style.current.subHeaderPointSize
        }

        ScrollView {
            anchors.top: participantLabel.bottom
            anchors.right: parent.right
            anchors.bottom: selfAction.top
            anchors.left: parent.left
            anchors.topMargin: 30
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            anchors.bottomMargin: 30
            clip: true

            ColumnLayout {
                id: participantColumns
                anchors.fill: parent
                visible: overviewVisible || overviewAnimation.running

                Repeater {
                    model: overviewContainer.roomModel.participants
                    delegate: participantOverviewComponent
                }
            }
        }

        Component {
            id: participantOverviewComponent
            ParticipantOverview {
                width: overviewWidth - 20
            }
        }

        ParticipantAction {
            id: selfAction
            anchors.horizontalCenter: parent.horizontalCenter
            visible: overviewVisible || overviewAnimation.running
            anchors.bottom: parent.bottom
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

