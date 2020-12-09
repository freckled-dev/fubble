import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.0
import "."

Item {
    property alias settings: settings

    Settings {
        id: settings
        property alias darkMode: darkModeSwitch.checked
        property alias showRoomHistory: roomHistorySwitch.checked
    }

    Item {
        id: settingItems
        anchors.fill: parent

        Label {
            id: uiTitle
            text: qsTr("User Interface")
            anchors.topMargin: 20
            font.pointSize: Style.current.largeTextPointSize
            width: settingsDialog.availableWidth
        }

        Switch {
            id: darkModeSwitch
            anchors.top: uiTitle.bottom
            text: qsTr("Enable dark mode")
            checked: settings.darkMode
            onCheckedChanged: {
                if (darkModeSwitch.checked) {
                    Style.current = Style.themeDark
                } else {
                    Style.current = Style.themeLight
                }
            }
        }

        Switch {
            id: roomHistorySwitch
            anchors.top: darkModeSwitch.bottom
            text: qsTr("Show the room history")
            checked: true || settings.saveRoomHistory
        }

        Label {
            id: languageTitle
            text: qsTr("Language Settings")
            anchors.topMargin: 20
            anchors.top: roomHistorySwitch.bottom
            font.pointSize: Style.current.largeTextPointSize
            width: settingsDialog.availableWidth
        }

        Item {
            id: item1
            anchors.topMargin: 10
            anchors.top: languageTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            implicitHeight: languageCombo.implicitHeight

            Label {
                id: languageLabel
                text: qsTr("Language")
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                font.pointSize: Style.current.textPointSize
            }

            ComboBox {
                id: languageCombo
                // does not work. i guess because the property is read only
                currentIndex: languageModel.languageIndex
                onCurrentIndexChanged: languageModel.languageIndex = currentIndex
                textRole: "description"
                anchors.left: languageLabel.right
                anchors.right: parent.right
                anchors.leftMargin: 20
                model: languageModel.languages
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

