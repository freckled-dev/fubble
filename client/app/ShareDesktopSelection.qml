import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import QtMultimedia 5.0

Popup {
    id: popup

    parent: overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    modal: true
    focus: true

    Material.foreground: Style.current.foreground

    ColumnLayout {

        TabBar {
            id: bar

            Repeater {
                model: shareDesktopModel.categories

                TabButton {
                    text: name
                    width: implicitWidth
                }
            }
        }

        StackLayout {
            id: tabItem
            currentIndex: bar.currentIndex

            Repeater {
                model: shareDesktopModel.categories

                GridLayout {

                    Repeater {
                        model: previews

                        ColumnLayout {

                            Rectangle {
                                color: Style.current.transparent
                                border.color: Style.current.foreground
                                implicitHeight: video.height
                                implicitWidth: video.width

                                VideoOutput {
                                    id: video
                                    source: player
                                    height: 200
                                    width: 300
                                    fillMode: VideoOutput.Stretch
                                }
                            }

                            Label {
                                text: description
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }
                }
            }
        }
    }

    onOpened: shareDesktopModel.startPreviews()
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

