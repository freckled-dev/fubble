import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    property TextEdit textEdit
    property bool canCut: true
    property bool canCopy: true
    property bool canDelete: true
    property bool canPaste: true
    property bool canUndo: true
    property bool canRedo: true

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        hoverEnabled: true
        onClicked: {
            contextMenu.x = mouse.x
            contextMenu.y = mouse.y
            contextMenu.open()
        }

        Menu {
            id: contextMenu

            MenuItem {
                text: "Cut"
                visible: canCut
                height: canCut ? implicitHeight : 0
                onTriggered: {
                    textEdit.cut()
                }
            }

            MenuItem {
                text: "Copy"
                visible: canCopy
                height: canCopy ? implicitHeight : 0
                onTriggered: {
                    textEdit.copy()
                }
            }

            MenuItem {
                text: "Paste"
                visible: canPaste
                height: canPaste ? implicitHeight : 0
                onTriggered: {
                    textEdit.paste()
                }
            }

            MenuItem {
                text: "Delete"
                visible: canDelete
                height: canDelete ? implicitHeight : 0
                onTriggered: {
                    textEdit.remove(textEdit.selectionStart,
                                    textEdit.selectionEnd)
                }
            }

            MenuSeparator {
                visible: canUndo || canRedo
                height: canUndo || canRedo ? implicitHeight : 0
            }

            MenuItem {
                text: "Undo"
                enabled: textEdit.canUndo
                visible: canUndo
                height: canUndo ? implicitHeight : 0
                onTriggered: {
                    textEdit.undo()
                }
            }
            MenuItem {
                text: "Redo"
                enabled: textEdit.canRedo
                visible: canRedo
                height: canRedo ? implicitHeight : 0
                onTriggered: {
                    textEdit.redo()
                }
            }

            MenuSeparator {
                visible: canUndo || canRedo
                height: canUndo || canRedo ? implicitHeight : 0
            }

            MenuItem {
                text: "Select All"
                onTriggered: {
                    textEdit.selectAll()
                }
            }
        }
    }
}
