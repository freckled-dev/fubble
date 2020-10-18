import QtQuick 2.14
import QtQuick.Controls 2.14
import io.fubble 1.0
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.12
import "."

Popup {
    property LeaveModel leaveModel
    signal left
    signal leaving
    signal forceShutdown
    property bool showForceButton: true
    property alias progressText: progressLabel.text

    property bool isLeavePopup
    property bool isJoinPopup

    x: (container.width - width) / 2
    y: (container.height - height) / 3

    Material.foreground: Style.current.foreground
    width: 300
    height: 300
    modal: true
    topMargin: 1
    padding: 0
    closePolicy: Popup.NoAutoClose

    onOpened: {
        if (isLeavePopup) {
            leaveRoom()
        }
    }

    Connections {
        target: leaveModel
        onLeft: {
            close()
            left()
        }
    }

    Rectangle {
        id: rectangle
        radius: 5
        border.width: 1

        color: Style.current.background
        border.color: Style.current.foreground
        anchors.fill: parent

        AnimatedImage {
            id: animation
            anchors.top: parent.top
            source: Style.current.joinAnimation
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 10
        }

        Label {
            id: progressLabel
            font.pointSize: Style.current.subHeaderPointSize
            color: Style.current.foreground
            anchors.top: animation.bottom
            anchors.topMargin: force.visible ? 0 : 20
            anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: force
            Material.background: Style.current.gray700
            Material.foreground: Style.current.buttonTextColor
            Layout.alignment: Qt.AlignHCenter
            anchors.top: progressLabel.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Force close")
            visible: showForceButton && isLeavePopup
            onClicked: forceShutdown()
        }
    }

    function leaveRoom() {
        leaving()
        leaveModel.leave()
    }
}
