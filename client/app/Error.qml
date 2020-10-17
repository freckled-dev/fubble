import QtQuick 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import QtQuick.Controls.Material 2.12
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
        onError: {
            playErrorSound()
            open()
        }
    }

    Label {
        id: label
        text: errorModel.text
        textFormat: Text.RichText
        padding: 20
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Image {
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

