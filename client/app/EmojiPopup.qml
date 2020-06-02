import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "scripts/emoji.js" as EmojiJSON
import "."

Popup {
    id: emojiPopup
    property var emojiListModel: ListModel {
        ListElement {
            name: "favourites"
            description: "Recently used"
            emojis: []
        }
    }
    property bool shouldShow: false
    margins: 0
    padding: 0
    clip: true
    signal insertEmoji(string emoji)
    property var favouriteEmojis

    function initJson() {
        var emojiJson = JSON.parse(EmojiJSON.emoji_json)

        for (var i = 0; i < emojiJson.emoji_categories.length; i++) {
            var currentCategory = emojiJson.emoji_categories[i]

            var emojiList = emojiJson.emoji_by_category[currentCategory.name]
            emojiListModel.append({
                                      "name": currentCategory.name,
                                      "description": currentCategory.description,
                                      "emojis": []
                                  })

            for (var j = 0; j < emojiList.length; j++) {
                var currentEmoji = emojiList[j]
                emojiListModel.get(i + 1).emojis.append({
                                                            "value": currentEmoji
                                                        })
            }
        }
    }

    function initFavourites() {
        var emojiArray = favouriteEmojis.split(",")
        var noFavorites = emojiArray.length === 0 || emojiArray[0] === ""
        if (noFavorites) {
            return
        }

        emojiListModel.get(0).emojis.clear()
        // add recently used emojis
        for (var j = 0; j < emojiArray.length; j++) {
            var currentEmoji = emojiArray[j]
            if (currentEmoji !== "") {
                emojiListModel.get(0).emojis.append({
                                                        "value": currentEmoji
                                                    })
            }
        }
    }

    function hasEmojis(index) {
        var emojis = emojiListModel.get(index).emojis
        if (emojis === undefined) {
            return false
        }

        return emojis.count > 0
    }

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        border.width: 1
        border.color: Style.current.foreground
        anchors.fill: parent

        ListView {
            id: categoryRepeater
            model: emojiListModel
            anchors.fill: parent
            anchors.margins: 10
            clip: true
            cacheBuffer: 5000

            ScrollBar.vertical: ScrollBar {
                id: emojiScrollBar
                active: true
            }

            delegate: ColumnLayout {
                visible: hasEmojis(index)
                height: hasEmojis(index) ? undefined : 0

                Label {
                    text: {
                        return description
                    }
                    font.pointSize: Style.current.subHeaderPointSize
                }

                GridLayout {
                    id: emojiGrid
                    columns: 8
                    Repeater {
                        id: participantRepeater

                        model: emojis
                        Label {
                            id: emoji
                            text: index >= 0 ? emojis.get(index).value : ""

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
                                    insertEmoji(modelData)

                                    close()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    onShouldShowChanged: {
        if (shouldShow) {
            initJson()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

