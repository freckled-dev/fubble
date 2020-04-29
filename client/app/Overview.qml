import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0

Rectangle {
    color: "#cb2323"

    ColumnLayout {
        anchors.right: parent.right
        anchors.rightMargin: 300
        anchors.left: parent.left
        anchors.leftMargin: 0

        Repeater {
            model: layout.room.participants
            delegate: participantOverviewComponent
        }

        Component {
            id: participantOverviewComponent
            ParticipantOverview {}
        }
    }
}
