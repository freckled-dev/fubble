import QtQuick 2.0
import QtQuick.Controls 2.2
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.0
import "."

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
            id: element
            implicitHeight: roomNameLabel.height + enterTimeLabel.height + durationLabel.height
            implicitWidth: roomHistoryContainer.width

            Label {
                id: roomNameLabel
                text: roomName
                font.bold: true
            }

            Label {
                id: enterTimeLabel
                anchors.top: roomNameLabel.bottom
                text: new Date(enterTime).toLocaleDateString()
            }

            Label {
                id: durationLabel
                anchors.top: enterTimeLabel.bottom
                text: new Date(enterTime).toTimeString(
                          ) + " lasting " + toHHMMSS(duration)
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
                    property bool hoverDeleteIcon: false

                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: hoverDeleteIcon = true
                    onExited: hoverDeleteIcon = false
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!maDelete.hoverDeleteIcon) {
                        joinRoom(roomName)
                    } else {
                        deleteHistory(index)
                    }
                }
            }
        }
    }

    function deleteHistory(historyIndex) {
        dataModel.remove(historyIndex, 1)
        saveRoomHistory()
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

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

