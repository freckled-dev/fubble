import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import QtQuick.Controls.Material 2.15
import io.fubble 1.0
import "."

Popup {
    id: errorDialog
    property alias errorText: label.text
    property ErrorModel errorModel

    Material.foreground: Style.current.buttonTextColor
    Material.background: Style.current.accent
    modal: true

    //timeout: 5000
    y: container.height
    height: label.height
    padding: 0
    margins: 0
    width: container.width

    Connections {
        target: errorModel
        function onError() {
            playErrorSound()
            open()
        }
    }

    LabelWithLink {
        id: label
        text: errorModel.text
        textFormat: Text.RichText
        padding: 20
        onLinkActivated: Qt.openUrlExternally(link)
        anchors.right: closeButton.left
        anchors.left: parent.left
    }

    Image {
        id: closeButton
        anchors.top: parent.top
        anchors.topMargin: 8
        source: Style.current.closeImage
        anchors.right: parent.right
        anchors.rightMargin: 8

        MouseArea {
            anchors.fill: parent
            onClicked: {
                close()
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;height:100;width:500}
}
##^##*/

