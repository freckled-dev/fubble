import QtQuick 2.14

Theme {
    property color primary: '#0277BD'
    property color accent: '#fd7d3e'
    property color accentLight: '#ffe393'
    property color foreground: '#000000'
    property color black: '#000000'
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

    property color wildAppleGradientStart: '#d299c2'
    property color wildAppleGradientEnd: '#fef9d7'

    // Images
    property var closeImage: "images/close_light.svg"
    property var closeIcon: "images/close_icon.svg"
    property var copyImage: "images/copy.svg"
    property var copyPressedImage: "images/copy_pressed.svg"
    property var exitImage: "images/exit_light.gif"
    property var logoImage: "images/fubble.svg"
    property var mutedImage: "images/muted.svg"
    property var silencedImage: "images/silenced.svg"
    property var deafedImage: "images/deafed.svg"
    property var mutedOffImage: "images/muted_off.svg"
    property var videoDisabledImage: "images/video_disabled.svg"

    property var overlayMuteImage: "images/overlay_mute.svg"
    property var overlayMuteOffImage: "images/overlay_mute_off.svg"
    property var overlaySilenceImage: "images/overlay_silence.svg"
    property var overlaySilenceOffImage: "images/overlay_silence_off.svg"
    property var overlayVideoImage: "images/overlay_video.svg"
    property var overlayVideoOffImage: "images/overlay_video_off.svg"
    property var overlayShareDesktopImage: "images/overlay_share_desktop.svg"
    property var overlayStopShareImage: "images/overlay_stop_share.svg"

    property var settingsImage: "images/overlay_settings.svg"
    property var feedbackImage: "images/feedback.svg"

    property var collapseImageLeft: "images/left_arrow.svg"
    property var collapseImageRight: "images/right_arrow.svg"

    // Chat Images
    property var chatImage: "images/chat.svg"
    property var chatNewMessageImage: "images/chat_new.svg"
    property var chatPressedImage: "images/chat_pressed.svg"

    // Overview Images
    property var overviewImage: "images/overview.svg"
    property var overviewNewImage: "images/overview_new.svg"
    property var overviewNewPressedImage: "images/overview_new_pressed.svg"
    property var overviewPressedImage: "images/overview_pressed.svg"

    property var chatColors: ["#2eb82e", "#bc69aa", "#990033", "#577590", "#e6b800", "#a64dff", "#b6465f", "#034078", "#a5668b", "#415971", "#c9671d", "#a06cd5", "#57241d", "#00802b", "#694873", "#f94144"]

    // DemoMode
    property var demoImagesPath: "demo_images/"

    // Animations
    property var joinAnimation: "animations/bricks_light.gif"
}
