import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import io.fubble 1.0

ColumnLayout {

    property ParticipantModel participant

    Label {
        text: model.participant.name
    }
}
