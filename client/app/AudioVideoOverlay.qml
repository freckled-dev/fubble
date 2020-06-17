import QtQuick 2.0
import "."
import io.fubble 1.0

Rectangle {
    id: rectangle
    color: Style.current.gray300Transparent

    FubbleActionButton {
        id: settingsButton
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        showToolTip: false
        source: Style.current.settingsImage
        onActionClick: openSettings()
    }

    function openSettings() {
        fubbleSettings.bar.setCurrentIndex(1)
        fubbleSettings.open()
    }

    FubbleSettings {
        id: fubbleSettings
    }

    AudioChart {
        anchors.left: settingsButton.right
        anchors.right: videoOffButton.left
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        audioLevel: ownMediaModel.audioLevel
    }

    FubbleActionButton {
        id: videoOffButton
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: muteButton.left
        showToolTip: false
        source: ownMediaModel.videoDisabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage
        //toolTipText: qsTr("Disable your video")
        onActionClick: ownMediaModel.videoDisabled = !ownMediaModel.videoDisabled
    }

    FubbleActionButton {
        id: muteButton
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        source: ownMediaModel.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage
        anchors.right: parent.right
        showToolTip: false
        //toolTipText: qsTr("Mute yourself")
        onActionClick: ownMediaModel.muted = !ownMediaModel.muted
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

