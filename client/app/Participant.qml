import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0

Rectangle {
    radius: 5
    layer.enabled: true
    // border.color: Style.current.foreground
    property ParticipantModel participant
    color: Style.current.background

    ColumnLayout {
        anchors.fill: parent

        ToolBar {
            id: nameToolbar
            hoverEnabled: false
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                Label {
                    font.pixelSize: 20
                    Material.foreground: Style.current.buttonTextColor
                    text: participant.name
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }
        }

        VideoOutput {
            id: video
            source: participant.video
            visible: participant.videoAvailable
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

