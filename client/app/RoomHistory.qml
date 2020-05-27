import QtQuick 2.0
import QtQuick.Controls 2.2
import Qt.labs.settings 1.0

Item {
    id: roomHistoryContainer

    property string history: ""

    Settings {
        property alias roomHistory: roomHistoryContainer.history
    }

    Label {
        id: historyHeader
        text: qsTr("Room History")
        font.pointSize: Style.current.largeTextPointSize
    }

    ListView {
        id: view
        anchors.top: historyHeader.bottom
        height: childrenRect.height
        width: childrenRect.width
        anchors.topMargin: 10

        model: ListModel {
            id: dataModel
        }

        delegate: roomHistoryComponent
    }

    Component.onCompleted: loadRoomHistory()

    function loadRoomHistory() {
        if (history) {
            var datamodel = JSON.parse(history)
            for (var index = 0; index < datamodel.length; index++) {
                dataModel.append(datamodel[index])
            }
        }
    }

    function addRoomToHistory(newHistoryItem) {
        dataModel.insert(0, newHistoryItem)
        saveRoomHistory()
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
            implicitHeight: roomNameLabel.height

            Label {
                id: roomNameLabel
                text: roomName + ", " + new Date(enterTime).toTimeString(
                          ) + ", " + toHHMMSS(duration)
            }

            //            Label {
            //                id: enterTimeLabel
            //                text: new Date(enterTime).toTimeString()
            //            }

            //            Label {
            //                id: durationLabel
            //                text: duration
            //            }
        }
    }

    function toHHMMSS(timestring) {
        var sec_num = parseInt(timestring, 10)
        var hours = Math.floor(sec_num / 3600)
        var minutes = Math.floor((sec_num - (hours * 3600)) / 60)
        var seconds = sec_num - (hours * 3600) - (minutes * 60)

        if (hours < 10) {
            hours = "0" + hours
        }
        if (minutes < 10) {
            minutes = "0" + minutes
        }
        if (seconds < 10) {
            seconds = "0" + seconds
        }

        if (hours < 1) {
            return minutes + ':' + seconds
        }

        return hours + ':' + minutes + ':' + seconds
    }
}
