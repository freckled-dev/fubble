import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

Dialog {
    id: aboutDialog
    modal: true
    focus: true
    x: (container.width - width) / 2
    y: container.height / 6
    width: Math.min(container.width, container.height) / 3 * 2
    contentHeight: aboutColumn.height

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
            font.pixelSize: 16
        }

        Image {
            width: 100
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter
            source: "pics/Logo.svg"
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
    D{i:1;anchors_height:222}
}
##^##*/

