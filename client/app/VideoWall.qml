import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

Rectangle {

    id: videoWallContainer
    color: Style.current.background
    property RoomModel roomModel

    ColumnLayout {
        anchors.margins: 20
        anchors.fill: parent

        Label {
            text: qsTr("Video Wall")
            Layout.bottomMargin: 20
            font.pointSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        // remove once CHAT feature is availabe
        ColumnLayout {
            id: noVideos
            visible: roomModel.participantsWithVideo.length === 0

            Item {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                Layout.preferredHeight: animation.height

                AnimatedImage {
                    id: animation
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "pics/coming_soon.gif"
                }

                Button {
                    text: qsTr("Stop")
                    font.pointSize: 8
                    x: animation.x + animation.width - width - 5
                    y: animation.y

                    onClicked: {
                        var playing = animation.playing
                        animation.playing = !playing
                        if (playing)
                            text = qsTr("Start")
                        else
                            text = qsTr("Stop")
                    }
                }
            }

            Label {
                text: qsTr("Do not worry! In the next Fubble Version, there will be a Chat. It will fill this awful empty space, if no one is sharing a video :-)")
                Layout.fillWidth: true
                Layout.topMargin: 30
                font.pointSize: 14
            }
        }

        GridLayout {
            id: participantGrid
            Layout.fillWidth: true
            rowSpacing: 10
            columnSpacing: 10
            columns: videoWallContainer.calculateColumns()

            Repeater {
                model: roomModel.participantsWithVideo
                delegate: participantComponent
            }

            Component {
                id: participantComponent

                Participant {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    participant: model.participant
                }
            }
        }
    }

    function calculateColumns() {
        var aspectRatio = participantGrid.width / participantGrid.height
        var numParticipants = roomModel.participants.length
        // aspectRatio = aspectRatio * Math.sqrt(numParticipants)
        aspectRatio = Math.round(aspectRatio)
        aspectRatio = Math.max(1, aspectRatio)
        return aspectRatio
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;anchors_height:21;anchors_width:640}
}
##^##*/

