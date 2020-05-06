import "."
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import io.fubble 1.0

Item {
    id: element

    Label {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        font.pointSize: 14
        horizontalAlignment: Text.AlignHCenter
        id: titleLabel
        text: qsTr("Chat")
    }

    ListView {
        ScrollBar.vertical: ScrollBar {
            id: chatScrollBar
        }
        anchors.bottom: chatInput.top
        anchors.bottomMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleLabel.bottom
        clip: true
        delegate: chatDelegate
        id: chatList
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

        ChatMessage {
            anchors.right: own ? parent.right : undefined
        }
    }

    ChatInput {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        id: chatInput
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

