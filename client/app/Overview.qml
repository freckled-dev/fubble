import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
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
            anchors.bottom: actionLoader.top
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

        Loader {
            id: actionLoader
            anchors.horizontalCenter: parent.horizontalCenter
            sourceComponent: roomModel.ownParticipant ? actionComponent : undefined
            visible: overviewVisible || overviewAnimation.running
            anchors.bottom: parent.bottom
        }

        Component {
            id: actionComponent

            ParticipantAction {
                id: selfAction
                imageSize: 40
                participant: roomModel.ownParticipant
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

