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
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    Image {
        id: shareDesktopImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: Style.current.shareDesktopImage

        FubbleToolTip {
            id: ttShareDesktop
            text: qsTr("Share your screen")
            visible: maShareDesktop.containsMouse
        }

        MouseArea {
            id: maShareDesktop
            anchors.fill: parent
            hoverEnabled: true
            onClicked: shareDesktopPopup.open()
        }
    }

    ShareDesktopSelection {
        id: shareDesktopPopup
    }

    Image {
        id: videoDisabledImage
        sourceSize.height: imageSize
        sourceSize.width: imageSize
        source: participant.videoDisabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage

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
        source: participant.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage

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
        source: participant.silenced ? Style.current.overlaySilenceOffImage : Style.current.overlaySilenceImage

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
