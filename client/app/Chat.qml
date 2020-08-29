import "."
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.settings 1.0
import QtQuick.Layouts 1.12
import QtQml.Models 2.12
import io.fubble 1.0
import "scripts/utils.js" as Utils

Item {
    id: chatContainer
    property ChatModel chatModel
    property bool chatVisible: true
    property int chatWidth: 400
    property var chatParticipants

    property string recentlyUsedEmojis

    Connections {
        target: chatModel
        onNewMessagesChanged: {
            if (chatVisible && chatModel.newMessages > 0) {
                chatModel.resetNewMessages()
            }
        }
    }

    Settings {
        id: settings
        property alias recentlyUsedEmojis: chatContainer.recentlyUsedEmojis
    }

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
            cacheBuffer: 10000 // pixels to fit the delegates

            model: delegateModel
            snapMode: ListView.SnapToItem
            spacing: 10

            Component.onCompleted: initialized = true

            onCountChanged: {
                if (initialized) {
                    scrollToBottom()
                }
            }

            onHeightChanged: {
                if (initialized) {
                    scrollToBottom()
                }
            }
        }

        DelegateModel {
            id: delegateModel
            model: chatModel.messages
            delegate: chatDelegate
            items.onChanged: {
                var insertedItem = delegateModel.items.get(
                            inserted[0].index).model

                switch (insertedItem.type) {
                case "leave":
                    playLeaveSound()
                    break
                case "join":
                    playJoinSound()
                    break
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
                participantColor: own ? Style.current.accent : chatContainer.getColorForParticipant(
                                            participantId)
            }
        }

        ChatInput {
            id: chatInput
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }

        EmojiPopup {
            id: emojiPopup

            height: 400
            width: 350
            x: chatHolder.width - width
            y: chatHolder.height - height - chatInput.textArea.height - 10
            favouriteEmojis: settings.recentlyUsedEmojis

            onOpened: {
                emojiPopup.initFavourites()
            }
            onClosed: {
                chatInput.textArea.forceActiveFocus()
            }

            onInsertEmoji: {
                var cursorPosition = chatInput.textArea.cursorPosition
                chatInput.textArea.insert(cursorPosition, emoji)
                addEmoji(emoji)
            }

            function addEmoji(emoji) {
                var emojiArray = recentlyUsedEmojis.split(",")

                // remove emoji, if it is already in the array
                for (var index = 0; index < emojiArray.length; index++) {
                    var current = emojiArray[index]
                    if (emoji === current) {
                        emojiArray.splice(index, 1)
                    }
                }

                // add emoji at the array start
                emojiArray.unshift(emoji)

                // allow a maximum of 16 emojis -> remove last one
                if (emojiArray.length === 17) {
                    emojiArray.splice(16, 1)
                }

                recentlyUsedEmojis = emojiArray.toString()
            }
        }
    }

    property variant colorMap: {
        "": ""
    }

    function getColorForParticipant(participantId) {
        var colorFound = colorMap[participantId]
        if (colorFound) {
            return colorFound
        }

        var size = Object.keys(colorMap).length
        var newColor = Style.current.chatColors[size - 1]

        // we have 16 predefind colors for dark and light mode
        if (!newColor) {
            newColor = Utils.getRandomColor()
        }

        colorMap[participantId] = newColor

        return newColor
    }

    onChatVisibleChanged: {
        if (chatVisible) {
            chatModel.resetNewMessages()
        }
        scrollToBottom()
    }

    function scrollToBottom() {
        chatScrollBar.setPosition(1)
    }
}
