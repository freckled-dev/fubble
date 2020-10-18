import QtMultimedia 5.14
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.14
import "."

RowLayout {
    id: actionOverlay

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    FubbleActionButton {
        icon.source: Style.current.overlayShareDesktopImage
        toolTipText: qsTr("Share your screen")
        onActionClick: shareDesktopPopup.open()
    }

    ShareDesktopSelection {
        id: shareDesktopPopup
    }

    FubbleActionButton {
        icon.source: Style.current.overlayStopShareImage
        toolTipText: qsTr("Stop desktop sharing")
        onActionClick: shareDesktopModel.stopShareDesktop()
        visible: shareDesktopModel.desktopSharingActive
    }
    FubbleActionButton {
        icon.source: ownMediaModel.videoDisabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage
        toolTipText: ownMediaModel.videoDisabled ? qsTr("Enable your video") : qsTr(
                                                       "Disable your video")
        onActionClick: ownMediaModel.videoDisabled = !ownMediaModel.videoDisabled
        visible: ownMediaModel.videoAvailable
    }

    FubbleActionButton {
        icon.source: ownMediaModel.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage
        toolTipText: ownMediaModel.muted ? qsTr("Unmute yourself") : qsTr(
                                               "Mute yourself")
        onActionClick: ownMediaModel.muted = !ownMediaModel.muted
    }

    FubbleActionButton {
        icon.source: ownMediaModel.deafed ? Style.current.overlaySilenceOffImage : Style.current.overlaySilenceImage
        toolTipText: ownMediaModel.deafed ? qsTr("Undeafen") : qsTr("Deafen")
        onActionClick: ownMediaModel.deafed = !ownMediaModel.deafed
    }
}
