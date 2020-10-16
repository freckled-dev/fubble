import QtQuick 2.0

TextEdit {
    id: chatMessage
    readOnly: true
    selectByMouse: true
    font.family: verdanaFont.name + ", " + emojiOneFont.name
    selectionColor: Style.current.accent
    selectedTextColor: Style.current.background

    textFormat: Text.MarkdownText
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    onLinkActivated: Qt.openUrlExternally(link)
    color: Style.current.foreground

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
