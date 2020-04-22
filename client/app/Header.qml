import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12

ToolBar {
    property string title

    /*
    Action {
        id: optionsMenuAction
        // icon.name: "icon should be part of theme!"
        onTriggered: optionsMenu.open()
    }
    */
    RowLayout {
        spacing: 20
        anchors.fill: parent

        ToolButton {
            // text: qsTr("‹ back")
            // action: navigateBackAction
        }

        Label {
            id: titleLabel
            text: title
            font.pixelSize: 20
            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        ToolButton {
            onClicked: optionsMenu.open()
            text: qsTr("⋮")
            // this nice cog.svg works with qt5.10 onwards :(
            // icon.source: "cog.svg"

            Menu {
                id: optionsMenu
                x: parent.width - width
                transformOrigin: Menu.TopRight
                MenuItem {
                    text: "About"
                    onTriggered: aboutDialog.open()
                }
            }
        }
    }
    Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: "About"
        x: (container.width - width) / 2
        y: container.height / 6
        width: Math.min(container.width, container.height) / 3 * 2
        contentHeight: aboutColumn.height

        Column {
            id: aboutColumn
            spacing: 20

            Label {
                width: aboutDialog.availableWidth
                text: "Fubble by <i>Freckled OG</i>.<br><a href='https://freckled.dev/contact'>Contact us at freckled.dev/contact</a>"
                onLinkActivated: Qt.openUrlExternally(link)
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }
}

