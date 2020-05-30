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
                id: headerLabel
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.topMargin: 10
                color: own ? Style.current.accent : Style.current.primary
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
                var formattedMessage = "<style>a:link { color: "
                        + Style.current.linkColor + "; }</style>" + message
                formattedMessage = formattedMessage.replace(/\n/g, "<br />")

                var emojiRanges = ['(\u00a9|\u00ae|[\u2000-\u3300]|\ud83c[\ud000-\udfff]|\ud83d[\ud000-\udfff]|\ud83e[\ud000-\udfff])']
                var linkRange = /(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)(\/)?)/gi

                var emojiRegex = new RegExp(emojiRanges, 'gi')
                var linkRegex = new RegExp(linkRange)

                var removeEmoji = formattedMessage.replace(emojiRegex,
                                                           "").replace(/ /g, "")
                var isEmojiMessage = removeEmoji.length === 0

                formattedMessage = formattedMessage.replace(
                            linkRegex, '<a href=$1 title="">$1</a>')
                if (isEmojiMessage) {
                    // enlarge more if there are only emojis
                    formattedMessage = formattedMessage.replace(
                                emojiRegex,
                                '<span style="font-size:25pt">$1</span>')
                    return formattedMessage
                }

                // enlarge the emojis
                formattedMessage = formattedMessage.replace(
                            emojiRegex,
                            '<span style="font-size:18pt">$1</span>')

                return formattedMessage
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

