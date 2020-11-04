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
            text: qsTr("Language")
            anchors.topMargin: 20
            anchors.top: roomHistorySwitch.bottom
            font.pointSize: Style.current.largeTextPointSize
            width: settingsDialog.availableWidth
        }
        ComboBox {
            // does not work. i guess because the property is read only
            currentIndex: languageModel.languageIndex
            onCurrentIndexChanged: languageModel.languageIndex = currentIndex
            textRole: "description"
            anchors.top: languageTitle.bottom
            model: languageModel.languages
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

