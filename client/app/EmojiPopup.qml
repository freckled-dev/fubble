import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "emoji/emoji.js" as EmojiJSON
import "."

Popup {
    id: emojiPopup
    property var emojiJson: "{\"emoji_categories\": []"
    property TextArea textArea
    margins: 0
    padding: 0
    clip: true

    function initJson() {
        var jsonObject = JSON.parse(EmojiJSON.emoji_json)
        emojiJson = jsonObject
    }

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        border.width: 1
        border.color: Style.current.foreground
        anchors.fill: parent

        ScrollView {
            id: emojiScrollView
            anchors.fill: parent
            anchors.margins: 10
            clip: true

            ColumnLayout {
                spacing: 20
                Repeater {
                    id: categoryRepeater
                    model: emojiJson.emoji_categories

                    ColumnLayout {
                        Label {
                            text: modelData.description
                            font.pointSize: Style.current.subHeaderPointSize
                        }

                        GridLayout {
                            id: emojiGrid
                            columns: 8
                            Repeater {
                                id: participantRepeater
                                model: {
                                    return emojiJson.emoji_by_category[modelData.name]
                                }

                                Label {
                                    id: emoji
                                    text: modelData
                                    font.pointSize: 20
                                    padding: 4

                                    Rectangle {
                                        id: emojiBackground
                                        color: Style.current.transparent
                                        radius: 5
                                        border.width: 1
                                        anchors.fill: parent
                                        visible: false
                                    }

                                    MouseArea {
                                        id: mouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onEntered: emojiBackground.visible = true
                                        onExited: emojiBackground.visible = false
                                        onClicked: {
                                            textArea.insert(
                                                        textArea.cursorPosition,
                                                        modelData)
                                            close()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    onEnabledChanged: {
        if (enabled) {
            initJson()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

