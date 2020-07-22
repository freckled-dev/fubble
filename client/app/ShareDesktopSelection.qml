import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtMultimedia 5.12
import "."

Popup {
    id: popup

    parent: overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    modal: true
    focus: true
    padding: 0

    Material.foreground: Style.current.foreground

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        anchors.fill: parent
        implicitHeight: shareColumnLayout.height
        implicitWidth: shareColumnLayout.width
        border.width: 1
        border.color: Style.current.white

        ColumnLayout {
            id: shareColumnLayout

            TabBar {
                id: bar
                Layout.topMargin: 20
                Layout.rightMargin: 20
                Layout.leftMargin: 20
                Layout.bottomMargin: 10

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
                Layout.bottomMargin: 20
                Layout.rightMargin: 20
                Layout.leftMargin: 20

                Repeater {
                    model: shareDesktopModel.categories

                    GridLayout {

                        Repeater {
                            model: previews

                            Rectangle {
                                id: previewRect
                                color: Style.current.transparent
                                border.width: maPreview.containsMouse ? 2 : 1
                                border.color: maPreview.containsMouse ? Style.current.accent : Style.current.foreground

                                implicitHeight: video.height + descriptionLabel.height + 40
                                implicitWidth: video.width + 10

                                VideoOutput {
                                    id: video
                                    source: player
                                    height: 200
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: 10
                                    width: 300
                                    fillMode: VideoOutput.Stretch
                                }

                                Label {
                                    id: descriptionLabel
                                    text: description
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: video.bottom
                                    anchors.topMargin: 20
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                MouseArea {
                                    id: maPreview
                                    hoverEnabled: true
                                    anchors.fill: parent
                                    onClicked: shareDesktopModel.shareDesktop(
                                                   model)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    onOpened: shareDesktopModel.startPreviews()
    onClosed: shareDesktopModel.stopPreviews()
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

