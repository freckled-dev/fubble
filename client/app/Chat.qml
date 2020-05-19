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
    property int chatWidth: 300
    width: chatVisible ? chatWidth : 0

    Behavior on width {
        PropertyAnimation {
            id: chatAnimation
        }
    }

    ListView {
        id: chatList
        ScrollBar.vertical: ScrollBar {
            id: chatScrollBar
        }
        anchors.bottom: chatInput.top
        anchors.bottomMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        clip: true
        delegate: chatDelegate

        model: chatModel.messages
        snapMode: ListView.SnapToItem
        spacing: 10

        onCountChanged: {
            updateCurrentIndex()
        }

        onHeightChanged: {
            updateCurrentIndex()
        }

        function updateCurrentIndex() {
            chatScrollBar.setPosition(1)
        }
    }

    Component {
        id: chatDelegate

        ChatItem {
            anchors.right: own && type === "message" ? parent.right : undefined
            anchors.horizontalCenter: type !== "message" ? parent.horizontalCenter : undefined
            rectangleBorder.width: type === "message" ? 1 : 0
        }
    }

    ChatInput {
        id: chatInput
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        visible: chatVisible || chatAnimation.running
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
            width: 370
            x: chatContainer.width - width
            y: chatContainer.height - height - textArea.height - 10

            textArea: chatInput.textArea
            onClosed: {
                chatInput.textArea.forceActiveFocus()
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

