import QtMultimedia 5.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import io.fubble 1.0

Grid {
  property ParticipantModel participant
  columns: 1
  Label {
    text: participant.name
  }
}
