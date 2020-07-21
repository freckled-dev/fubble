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
        source: ownMediaModel.videoDisabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage
        toolTipText: ownMediaModel.videoDisabled ? qsTr("Enable your video") : qsTr(
                                                       "Disable your video")
        onActionClick: ownMediaModel.videoDisabled = !ownMediaModel.videoDisabled
    }

    FubbleActionButton {
        source: ownMediaModel.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage
        toolTipText: ownMediaModel.muted ? qsTr("Unmute yourself") : qsTr(
                                               "Mute yourself")
        onActionClick: ownMediaModel.muted = !ownMediaModel.muted
    }

    FubbleActionButton {
        source: ownMediaModel.deafed ? Style.current.overlaySilenceOffImage : Style.current.overlaySilenceImage
        toolTipText: ownMediaModel.deafed ? qsTr("Hear everyone") : qsTr(
                                                "Silence everyone")
        onActionClick: ownMediaModel.deafed = !ownMediaModel.deafed
    }
}
