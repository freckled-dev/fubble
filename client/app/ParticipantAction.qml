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

    spacing: 10
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    FubbleActionButton {
        source: Style.current.shareDesktopImage
        toolTipText: qsTr("Share your screen")
        onActionClick: shareDesktopPopup.open()
    }

    ShareDesktopSelection {
        id: shareDesktopPopup
    }

    FubbleActionButton {
        source: !participant.videoEnabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage
        toolTipText: !participant.videoEnabled ? qsTr("Enable your video") : qsTr(
                                                     "Disable your video")
        onActionClick: participant.videoEnabled = !participant.videoEnabled
    }

    FubbleActionButton {
        source: participant.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage
        toolTipText: participant.muted ? qsTr("Unmute yourself") : qsTr(
                                             "Mute yourself")
        onActionClick: participant.muted = !participant.muted
    }

    FubbleActionButton {
        source: participant.deafed ? Style.current.overlaySilenceOffImage : Style.current.overlaySilenceImage
        toolTipText: participant.deafed ? qsTr("Hear everyone") : qsTr(
                                              "Silence everyone")
        onActionClick: participant.deafed = !participant.deafed
    }
}
