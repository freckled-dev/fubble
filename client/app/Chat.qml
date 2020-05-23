import "."
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import io.fubble 1.0
import "emoji"

Item {
    id: chatContainer
    property ChatModel chatModel
    property bool chatVisible: true
    property int chatWidth: 400
    signal newMessage

    Item {
        id: chatHolder
        anchors.fill: parent
        anchors.margins: 10
        visible: chatVisible || chatAnimation.running

        ListView {
            id: chatList
            property bool initialized: false

            ScrollBar.vertical: ScrollBar {
                id: chatScrollBar
            }
            clip: true

            anchors.bottom: chatInput.top
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            delegate: chatDelegate

            model: chatModel.messages
            snapMode: ListView.SnapToItem
            spacing: 10

            Component.onCompleted: initialized = true

            onCountChanged: {
                if (initialized) {
                    scrollToBottom()
                    newMessage()
                }
            }

            onHeightChanged: {
                if (initialized) {
                    scrollToBottom()
                }
            }
        }

        Component {
            id: chatDelegate

            ChatItem {
                anchors.right: own
                               && type === "message" ? parent.right : undefined
                anchors.horizontalCenter: type !== "message" ? parent.horizontalCenter : undefined
                rectangleBorder.width: type === "message" ? 1 : 0
            }
        }

        ChatInput {
            id: chatInput
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Loader {
            id: emojiLoader
            sourceComponent: emojiComponent
            asynchronous: true
            onLoaded: chatInput.smileyButton.enabled = true
        }

        Component {
            id: emojiComponent

            EmojiPopup {
                id: emojiPopup

                height: 400
                width: 350
                x: chatHolder.width - width
                y: chatHolder.height - height - textArea.height - 10

                textArea: chatInput.textArea
                onClosed: {
                    chatInput.textArea.forceActiveFocus()
                }
            }
        }
    }

    onChatVisibleChanged: {
        scrollToBottom()
    }

    function scrollToBottom() {
        chatScrollBar.setPosition(1)
    }
}
