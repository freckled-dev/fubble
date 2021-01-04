import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.15
import "."
import "scripts/utils.js" as Utils

Item {
    id: roomHistoryContainer

    property string history: ""
    property bool hasRoomHistory: historyList.count > 0

    signal joinRoom(string roomName)

    Settings {
        id: settings
        property alias roomHistory: roomHistoryContainer.history
    }

    Label {
        id: historyHeader
        text: qsTr("Room History")
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: Style.current.largeTextPointSize
    }

    ScrollView {
        anchors.top: historyHeader.bottom
        anchors.topMargin: 20
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ListView {
            id: historyList
            anchors.fill: parent
            spacing: 20

            model: ListModel {
                id: dataModel
            }

            delegate: roomHistoryComponent
        }
    }

    Component.onCompleted: loadRoomHistory()

    function loadRoomHistory() {
        if (history) {
            var parsed = JSON.parse(history)
            for (var index = 0; index < parsed.length; index++) {
                dataModel.append(parsed[index])
            }
        }
    }

    function addRoomToHistory(newHistoryItem) {
        var historyIndex = getDuplicateEntryIndex(newHistoryItem)
        if (historyIndex !== undefined && historyIndex >= 0) {
            dataModel.remove(historyIndex)
        }

        dataModel.insert(0, newHistoryItem)
        saveRoomHistory()
    }

    function getDuplicateEntryIndex(newHistoryItem) {
        for (var index = 0; index < dataModel.count; index++) {
            var currentItem = dataModel.get(index)
            if (currentItem.roomName === newHistoryItem.roomName) {
                return index
            }
        }
    }

    function saveRoomHistory() {
        var histories = []
        for (var index = 0; index < dataModel.count; index++) {
            histories.push(dataModel.get(index))
        }
        history = JSON.stringify(histories)
    }

    Component {
        id: roomHistoryComponent

        Item {
            id: element
            implicitHeight: roomNameLabel.height + enterTimeLabel.height + durationLabel.height
            implicitWidth: roomHistoryContainer.width

            Label {
                id: roomNameLabel
                text: roomName
                font.bold: true
                wrapMode: "WrapAnywhere"
                anchors.left: parent.left
                anchors.right: parent.right
            }

            Label {
                id: enterTimeLabel
                anchors.top: roomNameLabel.bottom
                text: new Date(enterTime).toLocaleDateString()
                anchors.right: deleteHistoryIcon.left
                anchors.rightMargin: 10
                elide: Text.ElideRight
                clip:true
                anchors.left: parent.left
            }

            Label {
                id: durationLabel
                anchors.top: enterTimeLabel.bottom
                text: new Date(enterTime).toTimeString(
                          ) + " lasting " + Utils.toHHMMSS(duration)
            }

            MouseArea {
                anchors.fill: parent
                onClicked: joinRoom(roomName)
            }

            Image {
                id: deleteHistoryIcon
                height: 20
                anchors.rightMargin: 40
                anchors.verticalCenter: parent.verticalCenter
                width: 20
                source: Style.current.closeIcon
                anchors.right: parent.right

                MouseArea {
                    id: maDelete
                    anchors.fill: parent
                    onClicked: deleteHistory(index)
                }
            }
        }
    }

    function deleteHistory(historyIndex) {
        dataModel.remove(historyIndex, 1)
        saveRoomHistory()
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

