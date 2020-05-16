import QtQuick 2.0

QtObject {
    property color primary
    property color accent
    property color foreground
    property color background
    property color buttonTextColor

    property color white: '#ffffff'
    property color transparent: '#00000000'

    property color backgroundTextInput
    property color placeholderTextColor

    property string emojiOneFontSrc: 'EmojiOneFont.ttf'
    property string verdanaFontSrc: 'VerdanaFont.ttf'

    property int textPointSize: 10
    property int headerPointSize: 16
    property int subHeaderPointSize: 14
}
