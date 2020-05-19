import QtQuick 2.0

Theme {
    property color primary: '#0277BD'
    property color accent: '#fd7d3e'
    property color foreground: '#000000'
    property color background: '#ffffff'
    property color buttonTextColor: '#ffffff'
    property color backgroundTextInput: '#E0E0E0'
    property color placeholderTextColor: '#757575'

    property color gray100: '#F5F5F5'
    property color gray200: '#EEEEEE'
    property color gray300: '#E0E0E0'
    property color gray400: '#BDBDBD'
    property color gray300Transparent: '#CCE0E0E0'
    property color gray700: '#616161'

    // images
    property var closeImage: "images/close_light.svg"
    property var copyImage: "images/copy.svg"
    property var copyPressedImage: "images/copy_pressed.svg"
    property var exitImage: "images/exit_light.gif"
    property var logoImage: "images/fubble.svg"
    property var mutedImage: "images/muted.svg"
    property var mutedOffImage: "images/muted_off.svg"
    property var overlayMuteImage: "images/overlay_mute.svg"
    property var overlayMuteOffImage: "images/overlay_mute_off.svg"
    property var overlaySilenceImage: "images/overlay_silence.svg"
    property var overlaySilenceOffImage: "imagesoverlay_silence_off.svg"
    property var overlayVideoImage: "images/overlay_video.svg"
    property var overlayVideoOffImage: "images/overlay_video_off.svg"
    property var videoDisabledImage: "images/video_disabled.svg"

    // Chat Images
    property var chatImage: "images/chat.svg"
    property var chatPressedImage: "images/chat_pressed.svg"

    // Overview Images
    property var overviewImage: "images/overview.svg"
    property var overviewPressedImage: "images/overview_pressed.svg"
}
