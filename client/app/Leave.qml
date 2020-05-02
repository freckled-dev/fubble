import QtQuick 2.0
import QtQuick.Controls 2.2
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import "."

Popup {
    property LeaveModel leaveModel
    signal left
    signal leaving
    signal forceShutdown
    property bool showForceButton: true

    x: (container.width - width) / 2
    y: container.height / 6
    Material.foreground: Style.current.foreground
    width: 400
    height: 350
    modal: true
    topMargin: 1
    padding: 0
    closePolicy: Popup.NoAutoClose

    onOpened: {
        leaveRoom()
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
        color: "#00000000"
        radius: 5
        border.width: 1
        border.color: "#ffffff"
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
            text: qsTr("Leaving the room...")
            font.pointSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        AnimatedImage {
            id: animation
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
            source: {
                if (Style.isLightTheme) {
                    "pics/spinner_wave_light.gif"
                } else {
                    "pics/spinner_wave_dark.gif"
                }
            }
        }

        Button {
            id: force
            Material.background: Style.current.primary
            Material.foreground: Style.current.buttonTextColor
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Force close")
            visible: showForceButton
            onClicked: forceShutdown()
        }
    }
}
