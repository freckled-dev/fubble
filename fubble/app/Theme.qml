import QtQuick 2.15

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

    property string emojiOneFontSrc: 'fonts/EmojiOneFont.ttf'
    property string verdanaFontSrc: 'fonts/VerdanaFont.ttf'

    property int subTextPointSize: 8
    property int textPointSize: 10
    property int largeTextPointSize: 12
    property int subHeaderPointSize: 14
    property int headerPointSize: 16
}
