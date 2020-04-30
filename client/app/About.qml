import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

Popup {
    id: aboutDialog
    modal: true
    focus: true
    x: (container.width - width) / 2
    y: container.height / 6
    width: 400
    height: 350
    padding: 0

    Rectangle {
        id: rectangle
        color: "#00000000"
        radius: 5
        border.width: 1
        border.color: "#ffffff"
        anchors.fill: parent
    }

    Column {
        id: aboutColumn
        height: 280
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 20

        Label {
            width: aboutDialog.availableWidth
            text: qsTr("About")
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 18
        }

        Image {
            width: 100
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter
            source: "pics/fubble.svg"
            fillMode: Image.PreserveAspectFit
        }

        Label {
            width: aboutDialog.availableWidth
            text: qsTr("Fubble")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 16
        }

        Label {
            width: aboutDialog.availableWidth
            text: qsTr("by Freckled OG")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }

        Label {
            width: aboutDialog.availableWidth
            text: qsTr("<a href='https://freckled.dev/contact'>Contact us...</a>")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:200;anchors_width:200;anchors_x:41;anchors_y:29}D{i:2;anchors_height:222}
}
##^##*/

