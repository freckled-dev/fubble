import QtMultimedia 5.0
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

RowLayout {
    id: actionOverlay
    property int imageSize: 40
    property bool videoOff: false
    property bool muted: false

    spacing: 10
    anchors.bottomMargin: 20
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    Image {
        id: videoOffImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: videoOff ? "pics/overlay_video_off.svg" : "pics/overlay_video.svg"

        ToolTip {
            id: ttVideoOff
            text: videoOff ? qsTr("Turn on your video") : qsTr(
                                 "Turn off your video")
            visible: maVideoOff.containsMouse
        }

        MouseArea {
            id: maVideoOff
            anchors.fill: parent
            hoverEnabled: true
            onClicked: videoOff = !videoOff
        }
    }

    Image {
        id: muteImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: muted ? "pics/overlay_mute_off.svg" : "pics/overlay_mute.svg"

        ToolTip {
            id: ttMute
            text: muted ? qsTr("Unmute yourself") : qsTr("Mute yourself")
            visible: maMute.containsMouse
        }

        MouseArea {
            id: maMute
            anchors.fill: parent
            hoverEnabled: true
            onClicked: muted = !muted
        }
    }
}
