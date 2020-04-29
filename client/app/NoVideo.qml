import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtMultimedia 5.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

Rectangle {
    color: "#00000000"
    radius: 5
    border.color: Style.current.foreground
    border.width: 1
    Material.foreground: Style.current.foreground
    Layout.fillWidth: true

    Flipable {
        id: noImageFlipable
        property bool flipped: false
        anchors.rightMargin: 40
        anchors.leftMargin: 40
        anchors.fill: parent

        front: ColumnLayout {
            id: noImageColumnLayout
            spacing: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.left

            Label {
                text: qsTr("No video camera found.")
                font.pointSize: 14
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                font.bold: true
                wrapMode: Text.WordWrap
            }
            Label {
                text: qsTr("Please check your video camera and settings and restart the app or continue without video...")
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }
        back: Image {
            anchors.centerIn: parent
            height: 240
            width: 240
            source: "pics/no_video.svg"
        }

        transform: Rotation {
            id: rotation
            origin.x: noImageFlipable.width / 2
            origin.y: noImageFlipable.height / 2
            axis.x: 0
            axis.y: 1
            axis.z: 0 // set axis.y to 1 to rotate around y-axis
            angle: 0 // the default angle
        }

        states: State {
            name: "back"
            PropertyChanges {
                target: rotation
                angle: 180
            }
            when: noImageFlipable.flipped
        }

        transitions: Transition {
            NumberAnimation {
                target: rotation
                property: "angle"
                duration: 1000
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: noImageFlipable.flipped = !noImageFlipable.flipped
        }
    }
}
