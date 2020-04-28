import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0

GridLayout {
    id: layout
    columns: 2
    // rowSpacing: 20
    // columnSpacing: 20
    property RoomModel room
    property var title: layout.room.name

    Repeater {
        model: layout.room.participants
        delegate: participantComponent
    }

    Component {
        id: participantComponent
        Participant {
            Layout.margins: 10
            // Layout.fillWidth: true
            Layout.maximumWidth: container.width / 2
            Layout.maximumHeight: container.height / 2
            participant: model.participant
        }
    }
}
