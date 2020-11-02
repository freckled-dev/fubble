import QtQuick 2.15
import io.fubble 1.0
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import QtQuick.Layouts 1.15
import "."

Rectangle {
    id: rectangle
    property string participantColor
    property int maximumWidth
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
        height: childrenRect.height
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
                Layout.maximumWidth: maximumWidth
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: own ? Style.current.accent : participantColor
                text: name + " - " + new Date(timestamp).toTimeString()
            }

            SelectableLabel {
                id: chatMessage
                Layout.bottomMargin: 10
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.maximumWidth: maximumWidth
                font.pointSize: {
                    if (message.length > 5)
                        return 10
                    var emojiRanges = ['(\u00a9|\u00ae|[\u2000-\u3300]|\ud83c[\ud000-\udfff]|\ud83d[\ud000-\udfff]|\ud83e[\ud000-\udfff])']
                    var emojiRegex = new RegExp(emojiRanges, 'gi')
                    var removeEmoji = message.replace(emojiRegex, "")
                    var isOnlyEmojiMessage = removeEmoji.length === 0
                    if (isOnlyEmojiMessage)
                        return 25
                    return 10
                }
                text: message
            }
        }
    }

    Component {
        id: infoComponent

        ColumnLayout {
            Label {
                color: Style.current.foreground
                font.pointSize: Style.current.textPointSize
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                Layout.maximumWidth: maximumWidth

                text: {
                    switch (type) {
                    case "leave":
                        return new Date().toTimeString() + " - " + name + qsTr(" left the room")
                    case "join":
                        return new Date().toTimeString() + " - " + name + qsTr(" joined the room")
                    default:
                        return qsTr("")
                    }
                }
                horizontalAlignment: Text.AlignHCenter
                bottomPadding: 5
                topPadding: 5
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

