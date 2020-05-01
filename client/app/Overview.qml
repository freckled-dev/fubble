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
            font.pointSize: 14
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
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

