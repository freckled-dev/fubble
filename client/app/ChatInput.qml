import "."
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import io.fubble 1.0

Rectangle {
    id: rectangle
    property alias textArea: inputText
    property alias smileyButton: smileyButton
    radius: 5
    border.width: 1
    color: Style.current.backgroundTextInput
    border.color: Style.current.foreground
    implicitHeight: inputText.implicitHeight

    TextArea {
        id: inputText
        persistentSelection: true
        anchors.left: parent.left
        anchors.right: smileyButton.left
        background: null
        padding: 10
        topPadding: 20
        selectByMouse: true
        selectByKeyboard: true
        cursorVisible: true
        placeholderText: qsTr("Write your message here...")
        wrapMode: Text.WordWrap
        textFormat: TextEdit.PlainText

        Keys.onEnterPressed: {
            onInputFinished(event)
        }

        Keys.onReturnPressed: {
            onInputFinished(event)
        }

        FubbleContextMenu {
            textEdit: inputText
            anchors.fill: parent
        }
    }

    Button {
        id: smileyButton
        width: 40
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10
        visible: parent.width > 60

        contentItem: Text {
            font.family: emojiOneFont.name
            text: "😃"
            font.pointSize: 18
        }
        onClicked: emojiPopup.visible ? emojiPopup.close() : emojiPopup.open()
    }

    function onInputFinished(event) {
        if (inputText.text.length === 0) {
            return
        }

        if (event.modifiers === Qt.NoModifier
                || (event.modifiers === Qt.KeypadModifier
                    && event.key === Qt.Key_Enter)) {
            sendMessage()
        } else {
            event.accepted = false
        }
    }

    function sendMessage() {
        chatModel.sendMessage(inputText.text)
        inputText.clear()
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

