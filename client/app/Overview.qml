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

    Label {
        id: participantLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        text: qsTr("Participants")
        font.pointSize: Style.current.subHeaderPointSize
    }

    ColumnLayout {
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: participantLabel.bottom
        anchors.topMargin: 30
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        Repeater {
            model: overviewContainer.roomModel.participants
            delegate: participantOverviewComponent
        }
    }

    Component {
        id: participantOverviewComponent
        ParticipantOverview {}
    }

    ParticipantAction {
        id: selfAction
        imageSize: 40
        anchors.bottom: parent.bottom
        participant: roomModel.ownParticipant
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

