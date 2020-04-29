import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0

Rectangle {
    color: "#efd4d4"
    radius: 5
    border.color: "#3d556d"
    property ParticipantModel participant

    ColumnLayout {
        anchors.fill: parent

        ToolBar {
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                Label {
                    font.pixelSize: 20
                    Material.foreground: Style.current.buttonTextColor
                    text: participant.name
                    elide: Label.ElideRight
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                }
            }
        }

        //        VideoOutput {
        //            //source: videosModel.ownVideo
        //            source: participant.video
        //            visible: participant.videoAvailable
        //        }
        Image {
            Layout.fillWidth: true
            Layout.fillHeight: true
            fillMode: Image.PreserveAspectFit
            visible: !participant.videoAvailable
            source: "pics/no_video.svg"
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

