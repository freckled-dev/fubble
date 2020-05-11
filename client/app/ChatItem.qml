import QtQuick 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
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

    Loader {
        id: messageLoader

        sourceComponent: type === "message" ? chatComponent : infoComponent
    }

    Component {
        id: chatComponent

        ColumnLayout {
            id: chatColumn

            Label {
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.topMargin: 10
                color: own ? Style.current.accent : Style.current.primary
                text: name + " - " + new Date(timestamp).toTimeString()
            }

            Label {
                Layout.bottomMargin: 10
                Layout.leftMargin: 10
                Layout.maximumWidth: element.width * 2 / 3
                Layout.rightMargin: 10
                font.pointSize: isEmoji(
                                    message) ? 25 : Style.current.textPointSize
                text: message

                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            function isEmoji(str) {
                var ranges = ['(\u00a9|\u00ae|[\u2000-\u3300]|\ud83c[\ud000-\udfff]|\ud83d[\ud000-\udfff]|\ud83e[\ud000-\udfff])']
                var removeEmoji = str.replace(new RegExp(ranges, 'g'), '')

                return removeEmoji.length === 0
            }

            function replaceAll(str, find, replace) {
                return str.replace(new RegExp(find, 'g'), replace)
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
                        return qsTr("X has left the room...")
                    case "join":
                        return qsTr("X has joined the room...")
                    default:
                        return qsTr("")
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

