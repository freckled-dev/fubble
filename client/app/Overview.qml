import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import "."

Rectangle {
    id: overviewContainer
    color: Style.current.background
    property RoomModel roomModel

    ColumnLayout {
        id: columnLayout
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20

        Label {
            text: qsTr("Participants")
            Layout.bottomMargin: 20
            font.pointSize: Style.current.subHeaderPointSize
            Layout.alignment: Qt.AlignHCenter
        }

        Repeater {
            model: overviewContainer.roomModel.participants
            delegate: participantOverviewComponent
        }

        Component {
            id: participantOverviewComponent
            ParticipantOverview {
                Layout.fillWidth: true
            }
        }
    }

    ParticipantAction {
        id: selfAction
        imageSize: 40
        anchors.bottom: parent.bottom
        participant: getOwnParticipant()
    }

    function getOwnParticipant() {
        for (var index = 0; index < roomModel.participants.length; index++) {
            var current = roomModel.participants[index].participant
            if (current.own) {
                return current
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

