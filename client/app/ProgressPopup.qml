import QtQuick 2.12
import QtQuick.Controls 2.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.12
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
    height: 250
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
        gradient: Gradient {
            GradientStop {
                position: 0
                color: Style.current.wildAppleGradientStart
            }

            GradientStop {
                position: 1
                color: Style.current.wildAppleGradientEnd
            }
        }

        border.width: 1

        border.color: Style.current.background
        anchors.fill: parent
    }

    function leaveRoom() {
        leaving()
        leaveModel.leave()
    }

    ColumnLayout {
        anchors.topMargin: 40
        anchors.bottomMargin: 40
        anchors.fill: parent

        Label {
            id: progressLabel
            font.pointSize: Style.current.subHeaderPointSize
            color: Style.current.black
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: force
            Material.background: Style.current.gray700
            Material.foreground: Style.current.buttonTextColor
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Force close")
            visible: showForceButton && isLeavePopup
            onClicked: forceShutdown()
        }
    }
}
