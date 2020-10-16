import QtQuick 2.12

Theme {
    property color primary: '#82d5fa'
    property color accent: '#ffe393'
    property color accentLight: '#fd7d3e'
    property color foreground: '#ffffff'
    property color black: '#000000'
    property color background: '#303030'
    property color buttonTextColor: '#000000'
    property color backgroundTextInput: '#757575'
    property color placeholderTextColor: '#E0E0E0'
    property color linkColor: '#6290f9'

    property color gray100: '#212121'
    property color gray200: '#424242'
    property color gray300: '#616161'
    property color gray400: '#757575'
    property color gray300Transparent: '#CC616161'
    property color gray700: '#E0E0E0'

    property color wildAppleGradientStart: '#d299c2'
    property color wildAppleGradientEnd: '#fef9d7'

    // Images
    property var closeImage: "images/close_dark.svg"
    property var closeIcon: "images/close_icon_dark.svg"
    property var copyImage: "images/copy_dark.svg"
    property var copyPressedImage: "images/copy_pressed_dark.svg"
    property var exitImage: "images/exit_dark.gif"
    property var logoImage: "images/fubble.svg"
    property var mutedImage: "images/muted_dark.svg"
    property var silencedImage: "images/silenced_dark.svg"
    property var deafedImage: "images/deafed_dark.svg"
    property var mutedOffImage: "images/muted_off_dark.svg"
    property var videoDisabledImage: "images/video_disabled_dark.svg"

    property var overlayMuteImage: "images/overlay_mute.svg"
    property var overlayMuteOffImage: "images/overlay_mute_off.svg"
    property var overlaySilenceImage: "images/overlay_silence.svg"
    property var overlaySilenceOffImage: "images/overlay_silence_off.svg"
    property var overlayVideoImage: "images/overlay_video.svg"
    property var overlayVideoOffImage: "images/overlay_video_off.svg"
    property var overlayShareDesktopImage: "images/overlay_share_desktop.svg"
    property var overlayStopShareImage: "images/overlay_stop_share.svg"

    property var settingsImage: "images/overlay_settings.svg"
    property var feedbackImage: "images/feedback_dark.svg"

    // Chat Images
    property var chatImage: "images/chat_dark.svg"
    property var chatNewMessageImage: "images/chat_new_dark.svg"
    property var chatPressedImage: "images/chat_pressed_dark.svg"

    // Overview Images
    property var overviewImage: "images/overview_dark.svg"
    property var overviewNewImage: "images/overview_new_dark.svg"
    property var overviewNewPressedImage: "images/overview_new_pressed_dark.svg"
    property var overviewPressedImage: "images/overview_pressed_dark.svg"

    property var chatColors: ["#43aa8b", "#bc69aa", "#90be6d", "#577590", "#8f2d56", "#ffe6cc", "#b6465f", "#b4dafd", "#a5668b", "#7c98b3", "#edb183", "#a06cd5", "#ffffb3", "#8cd98c", "#694873", "#f94144"]

    // DemoMode
    property var demoImagesPath: "demo_images/"

    // Animations
    property var joinAnimation: "animations/bricks_dark.gif"
}
