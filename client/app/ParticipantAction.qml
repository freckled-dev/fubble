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
    property ParticipantModel participant
    property int imageSize: 40

    spacing: 10
    anchors.bottomMargin: 20
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    Image {
        id: videoDisabledImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: participant.videoDisabled ? "pics/overlay_video_off.svg" : "pics/overlay_video.svg"

        FubbleToolTip {
            id: ttVideoDisabled
            text: participant.videoDisabled ? qsTr("Enable your video") : qsTr(
                                                  "Disable your video")
            visible: maVideoDisabled.containsMouse
        }

        MouseArea {
            id: maVideoDisabled
            anchors.fill: parent
            hoverEnabled: true
            onClicked: participant.videoDisabled = !participant.videoDisabled
        }
    }

    Image {
        id: muteImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: participant.muted ? "pics/overlay_mute_off.svg" : "pics/overlay_mute.svg"

        FubbleToolTip {
            id: ttMute
            text: participant.muted ? qsTr("Unmute yourself") : qsTr(
                                          "Mute yourself")
            visible: maMute.containsMouse
        }

        MouseArea {
            id: maMute
            anchors.fill: parent
            hoverEnabled: true
            onClicked: participant.muted = !participant.muted
        }
    }

    Image {
        id: silenceImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: participant.silenced ? "pics/overlay_silence_off.svg" : "pics/overlay_silence.svg"

        FubbleToolTip {
            id: ttSilence
            text: participant.silenced ? qsTr("Hear everyone") : qsTr(
                                             "Silence everyone")
            visible: maSilence.containsMouse
        }

        MouseArea {
            id: maSilence
            anchors.fill: parent
            hoverEnabled: true
            onClicked: participant.silenced = !participant.silenced
        }
    }
}
