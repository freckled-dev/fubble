import "."
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import io.fubble 1.0

Rectangle {
    id: rectangle
    property alias textArea: inputText
    radius: 5
    color: Style.current.backgroundTextInput
    border.color: Style.current.foreground
    implicitHeight: inputText.implicitHeight

    TextArea {
        id: inputText
        anchors.left: parent.left
        anchors.right: smileyButton.left
        font.family: emojiFont.name
        font.pointSize: Style.current.normalPointSize
        background: null
        padding: 10
        topPadding: 20
        cursorVisible: true
        placeholderTextColor: Style.current.placeholderTextColor
        placeholderText: "Write your message here..."
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

        FontLoader {
            id: emojiFont
            source: "emoji/" + Style.current.emojiFontName
        }

        Keys.onEnterPressed: {
            onInputFinished(event)
        }

        Keys.onReturnPressed: {
            onInputFinished(event)
        }
    }

    Button {
        id: smileyButton
        text: qsTr("😃")
        font.family: emojiFont.name
        font.pointSize: 16
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 10
        width: 40
        anchors.right: parent.right
        onClicked: {
            return emojiPopup.visible ? emojiPopup.close() : emojiPopup.open()
        }
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

