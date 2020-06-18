import QtQuick 2.0
import "."
import io.fubble 1.0

Rectangle {
    id: rectangle
    color: Style.current.gray300Transparent
    property alias videoOffButtonVisible: videoOffButton.visible

    FubbleActionButton {
        id: settingsButton
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        source: Style.current.settingsImage
        onActionClick: openSettings()
        showToolTip: false
    }

    function openSettings() {
        fubbleSettings.bar.setCurrentIndex(1)
        fubbleSettings.open()
    }

    FubbleSettings {
        id: fubbleSettings
    }

    AudioChart {
        id: audioChart
        anchors.left: settingsButton.right
        anchors.right: videoOffButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        audioLevel: ownMediaModel.audioLevel

        chart.height: audioChart.height + 25
        chart.width: audioChart.width + 80
        chart.x: -30
        chart.y: -5
    }

    FubbleActionButton {
        id: videoOffButton
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: muteButton.left
        source: !ownMediaModel.videoEnabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage
        toolTipText: qsTr("Disable your video")
        onActionClick: ownMediaModel.videoEnabled = !ownMediaModel.videoEnabled
    }

    FubbleActionButton {
        id: muteButton
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        source: ownMediaModel.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage
        anchors.right: parent.right
        toolTipText: qsTr("Mute yourself")
        onActionClick: ownMediaModel.muted = !ownMediaModel.muted
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

