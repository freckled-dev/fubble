import QtQuick 2.0
import QtQuick.Controls 2.2

Item {
    property TextArea textInput

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
                onTriggered: {
                    textInput.cut()
                }
            }
            MenuItem {
                text: "Copy"
                onTriggered: {
                    textInput.copy()
                }
            }
            MenuItem {
                text: "Paste"
                onTriggered: {
                    textInput.paste()
                }
            }
            MenuItem {
                text: "Delete"
                onTriggered: {
                    textInput.remove(textInput.selectionStart,
                                     textInput.selectionEnd)
                }
            }
            MenuSeparator {}
            MenuItem {
                text: "Undo"
                enabled: textArea.canUndo
                onTriggered: {
                    textInput.undo()
                }
            }
            MenuItem {
                text: "Redo"
                enabled: textArea.canRedo
                onTriggered: {
                    textInput.redo()
                }
            }
            MenuSeparator {}
            MenuItem {
                text: "Select All"
                onTriggered: {
                    textInput.selectAll()
                }
            }
        }
    }
}
