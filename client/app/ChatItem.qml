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
                color: own ? Style.current.accent : participantColor
                text: name + " - " + new Date(timestamp).toTimeString()
            }

            Label {
                width: headerLabel.width
                Layout.bottomMargin: 10
                Layout.leftMargin: 10
                Layout.maximumWidth: chatContainer.width - 40
                Layout.rightMargin: 10
                text: modifyMessage(message)

                textFormat: Text.RichText
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                onLinkActivated: Qt.openUrlExternally(link)
            }

            function modifyMessage(message) {
                message = message.replace(/\n/g, "<br />")

                message = handleEmojis(message)
                message = handleLinks(message)

                return message
            }

            function handleLinks(message) {
                var linkRange = /(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)(\/)?)/gi

                var linkRegex = new RegExp(linkRange)
                var hasLink = message.match(linkRegex) !== null
                if (hasLink) {
                    message = "<style>a:link { color: "
                            + Style.current.linkColor + "; }</style>" + message
                }

                message = message.replace(linkRegex,
                                          '<a href=$1 title="">$1</a>')
                return message
            }

            function handleEmojis(message) {
                var emojiRanges = ['(\u00a9|\u00ae|[\u2000-\u3300]|\ud83c[\ud000-\udfff]|\ud83d[\ud000-\udfff]|\ud83e[\ud000-\udfff])']
                var emojiRegex = new RegExp(emojiRanges, 'gi')
                var removeEmoji = message.replace(emojiRegex,
                                                  "").replace(/ /g, "")
                var isOnlyEmojiMessage = removeEmoji.length === 0

                // enlarge more if it is an emoji only message
                if (isOnlyEmojiMessage) {
                    message = message.replace(
                                emojiRegex,
                                '<span style="font-size:25pt">$1</span>')
                    return message
                }

                // enlarge the emojis
                message = message.replace(
                            emojiRegex,
                            '<span style="font-size:18pt">$1</span>')

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
