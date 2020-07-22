import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.12
import "."

RowLayout {
    id: actionOverlay

    spacing: 10
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    FubbleActionButton {
        icon.source: Style.current.shareDesktopImage
        toolTipText: qsTr("Share your screen")
        onActionClick: shareDesktopPopup.open()
    }

    ShareDesktopSelection {
        id: shareDesktopPopup
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
        toolTipText: ownMediaModel.deafed ? qsTr("Hear everyone") : qsTr(
                                                "Silence everyone")
        onActionClick: ownMediaModel.deafed = !ownMediaModel.deafed
    }
}
