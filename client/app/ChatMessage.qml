import QtQuick 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "."

Rectangle {
    radius: 5
    border.color: Style.current.foreground
    border.width: 1
    implicitWidth: chatColumn.implicitWidth
    implicitHeight: chatColumn.implicitHeight
    Material.foreground: Style.current.foreground
    color: Style.current.background

    FontLoader {
        id: emojiFont
        source: "emoji/" + Style.current.emojiFontName
        //Component.onCompleted: console.log(name)
    }

    ColumnLayout {
        id: chatColumn

        Label {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 10
            color: own ? Style.current.accent : Style.current.primary
            font.family: emojiFont.name
            font.pointSize: Style.current.normalPointSize
            text: name + " - " + new Date(timestamp).toTimeString()
        }

        Label {
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.maximumWidth: element.width * 2 / 3
            Layout.rightMargin: 10
            font.family: emojiFont.name
            font.pointSize: isEmoji(
                                message) ? 25 : Style.current.normalPointSize
            text: message

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }

    function isEmoji(str) {
        var ranges = ['(?:[\u2700-\u27bf]|(?:\ud83c[\udde6-\uddff]){2}|[\ud800-\udbff][\udc00-\udfff]|[\u0023-\u0039]\ufe0f?\u20e3|\u3299|\u3297|\u303d|\u3030|\u24c2|\ud83c[\udd70-\udd71]|\ud83c[\udd7e-\udd7f]|\ud83c\udd8e|\ud83c[\udd91-\udd9a]|\ud83c[\udde6-\uddff]|[\ud83c[\ude01-\ude02]|\ud83c\ude1a|\ud83c\ude2f|[\ud83c[\ude32-\ude3a]|[\ud83c[\ude50-\ude51]|\u203c|\u2049|[\u25aa-\u25ab]|\u25b6|\u25c0|[\u25fb-\u25fe]|\u00a9|\u00ae|\u2122|\u2139|\ud83c\udc04|[\u2600-\u26FF]|\u2b05|\u2b06|\u2b07|\u2b1b|\u2b1c|\u2b50|\u2b55|\u231a|\u231b|\u2328|\u23cf|[\u23e9-\u23f3]|[\u23f8-\u23fa]|\ud83c\udccf|\u2934|\u2935|[\u2190-\u21ff])' // U+1F680 to U+1F6FF
                ]
        var removeEmoji = str.replace(new RegExp(ranges, 'g'), '')

        return removeEmoji.length === 0
    }

    function replaceAll(str, find, replace) {
        return str.replace(new RegExp(find, 'g'), replace)
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

