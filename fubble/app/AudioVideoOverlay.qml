import QtQuick 2.15
import "."
import io.fubble 1.0

Rectangle {
    id: rectangle
    color: Style.current.gray300Transparent

    property FubbleSettings settingsDialog
    property alias videoOffButtonVisible: videoOffButton.visible

    FubbleActionButton {
        id: settingsButton
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        icon.source: Style.current.settingsImage
        onActionClick: settingsDialog.open()
        showToolTip: false
    }

    Connections {
        target: ownMediaModel
        function onNewAudioLevel(level) {
            audioChart.addNewAudioLevel(level)
        }
    }

    AudioChart {
        id: audioChart
        anchors.left: settingsButton.right
        anchors.right: videoOffButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.topMargin: 5
        anchors.bottomMargin: 5
    }

    FubbleActionButton {
        id: videoOffButton
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: muteButton.left
        icon.source: ownMediaModel.videoDisabled ? Style.current.overlayVideoOffImage : Style.current.overlayVideoImage
        toolTipText: qsTr("Disable your video")
        onActionClick: ownMediaModel.videoDisabled = !ownMediaModel.videoDisabled
        visible: ownMediaModel.videoAvailable
    }

    FubbleActionButton {
        id: muteButton
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        icon.source: ownMediaModel.muted ? Style.current.overlayMuteOffImage : Style.current.overlayMuteImage
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

