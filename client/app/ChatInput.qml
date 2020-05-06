import "."
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import io.fubble 1.0

Rectangle {
    radius: 5
    color: Style.current.backgroundTextInput
    border.color: Style.current.foreground
    implicitHeight: inputText.implicitHeight

    TextArea {
        id: inputText
        anchors.left: parent.left
        anchors.right: parent.right
        background: null
        padding: 10
        topPadding: 20
        cursorVisible: true
        placeholderText: qsTr("Write your message here...")
        placeholderTextColor: Style.current.placeholderTextColor
        text: ""
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

        Keys.onEnterPressed: {
            onInputFinished(event)
        }

        Keys.onReturnPressed: {
            onInputFinished(event)
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

