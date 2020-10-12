import QtQuick 2.12
import io.fubble 1.0
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import QtQuick.Layouts 1.12
import "."

Rectangle {
    id: rectangle
    property alias rectangleBorder: rectangle.border
    radius: 5
    border.color: Style.current.foreground
    border.width: 1
    implicitWidth: messageLoader.implicitWidth
    implicitHeight: messageLoader.implicitHeight
    Material.foreground: Style.current.foreground
    color: Style.current.background

    property string participantColor

    Loader {
        id: messageLoader

        sourceComponent: type === "message" ? chatComponent : infoComponent
    }

    Component {
        id: chatComponent

        ColumnLayout {
            id: chatColumn

            Label {
                id: headerLabel
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.topMargin: 10
                Layout.alignment: own ? Qt.AlignRight : Qt.AlignLeft
                color: own ? Style.current.accent : participantColor
                text: name + " - " + new Date(timestamp).toTimeString()
            }

            SelectableLabel {
                id: chatMessage
                width: headerLabel.width
                Layout.bottomMargin: 10
                Layout.leftMargin: 10
                Layout.maximumWidth: chatContainer.width - 40
                Layout.rightMargin: 10
                text: message
            }
        }
    }

    Component {
        id: infoComponent

        ColumnLayout {
            id: infoColumn

            Label {
                Layout.margins: 10
                color: Style.current.foreground
                font.pointSize: Style.current.textPointSize
                text: {
                    switch (type) {
                    case "leave":
                        return qsTr(name + " has left the room...")
                    case "join":
                        return qsTr(name + " has joined the room...")
                    default:
                        return qsTr("")
                    }
                }
            }
        }
    }
}
