import QtQuick 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtMultimedia 5.0
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
                Layout.maximumWidth: chatContainer.width - 40
                Layout.rightMargin: 10
                text: modifyMessage(message)

                textFormat: Text.RichText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            function modifyMessage(message) {
                message = message.replace(/\n/g, "<br />")
                var ranges = ['(\u00a9|\u00ae|[\u2000-\u3300]|\ud83c[\ud000-\udfff]|\ud83d[\ud000-\udfff]|\ud83e[\ud000-\udfff])']
                var reg = new RegExp(ranges, 'g')
                var removeEmoji = message.replace(reg, "").replace(/ /g, "")
                var isEmojiMessage = removeEmoji.length === 0

                if (isEmojiMessage) {
                    // enlarge more if there are only emojis
                    message = message.replace(
                                reg, '<span style="font-size:25pt">$1</span>')
                    return message
                }

                // enlarge the emojis
                message = message.replace(
                            reg, '<span style="font-size:18pt">$1</span>')

                return message
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
                        playLeaveSound()
                        return qsTr(name + " has left the room...")
                    case "join":
                        playJoinSound()
                        return qsTr(name + " has joined the room...")
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

