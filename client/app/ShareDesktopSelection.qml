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

    property int videoWidth: 300
    property int videoHeight: 200
    property int videoWidthPadding: 10
    property int paddingHorizontal: 100
    property int paddingVertical: 200

    Material.foreground: Style.current.foreground

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        anchors.fill: parent
        implicitHeight: shareColumnLayout.implicitHeight
        implicitWidth: shareColumnLayout.implicitWidth
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

                    ScrollView {
                        id: previewScrollView

                        implicitHeight: Math.min(
                                            previewGrid.implicitHeight,
                                            container.height - paddingVertical)
                        clip: true

                        GridLayout {
                            id: previewGrid
                            columns: calculateColumns()

                            Repeater {
                                model: previews

                                Rectangle {
                                    id: previewRect
                                    color: Style.current.transparent
                                    border.width: maPreview.containsMouse ? 2 : 1
                                    border.color: maPreview.containsMouse ? Style.current.accent : Style.current.foreground

                                    implicitHeight: videoHeight + descriptionLabel.height + 40
                                    implicitWidth: videoWidth + videoWidthPadding

                                    VideoOutput {
                                        id: video
                                        source: player
                                        height: videoHeight
                                        width: videoWidth
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        anchors.top: parent.top
                                        anchors.topMargin: 10
                                        fillMode: VideoOutput.PreserveAspectFit
                                    }

                                    Label {
                                        id: descriptionLabel
                                        text: description
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: video.bottom
                                        anchors.rightMargin: 10
                                        anchors.leftMargin: 10
                                        anchors.topMargin: 20
                                        clip: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    MouseArea {
                                        id: maPreview
                                        hoverEnabled: true
                                        anchors.fill: parent
                                        onClicked: {
                                            shareDesktopModel.shareDesktop(
                                                        model.id)
                                            popup.close()
                                        }
                                    }
                                }
                            }

                            function calculateColumns() {
                                var columns = (container.width - paddingHorizontal)
                                        / (300 + videoWidthPadding)
                                return Math.min(4, columns)
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

