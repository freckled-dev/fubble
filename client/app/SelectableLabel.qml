import QtQuick 2.0

TextEdit {
    id: chatMessage
    readOnly: true
    text: modifyMessage(message)
    selectByMouse: true
    font.family: verdanaFont.name + ", " + emojiOneFont.name
    selectionColor: Style.current.accent

    textFormat: Text.RichText
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    onLinkActivated: Qt.openUrlExternally(link)

    FubbleContextMenu {
        textEdit: chatMessage
        anchors.fill: parent
        canCut: false
        canPaste: false
        canDelete: false
        canUndo: false
        canRedo: false
    }
}
