import QtQuick 2.12

QtObject {

    // index of chosen language in languages
    property int languageIndex: 1

    property var languages: ListModel {
        ListElement {
            languageId: "en"
            description: "English"
        }
        ListElement {
            languageId: "de"
            description: "Deutsch"
        }
    }
}
