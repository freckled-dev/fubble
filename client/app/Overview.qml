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
    property int overviewWidth: 200
    property bool overviewVisible: true
    width: overviewVisible ? overviewWidth : 0

    Behavior on width {
        PropertyAnimation {
            id: overviewAnimation
            //            onRunningChanged: {
            //                console.log(overviewAnimation.running)
            //            }
        }
    }

    Label {
        id: participantLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        text: qsTr("Participants")
        visible: overviewVisible || overviewAnimation.running
        font.pointSize: Style.current.subHeaderPointSize
    }

    ColumnLayout {
        anchors.right: parent.right
        anchors.left: parent.left
        visible: overviewVisible || overviewAnimation.running
        anchors.top: participantLabel.bottom
        anchors.topMargin: 30

        Repeater {
            model: overviewContainer.roomModel.participants
            delegate: participantOverviewComponent
        }
    }

    Component {
        id: participantOverviewComponent
        ParticipantOverview {}
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
