import QtQuick 2.12
import QtQml 2.0

QtObject {
    id: leaveContainer

    signal left(int reason)

    property var timer: Timer {
        id: timer
        interval: 1000
        running: false
        repeat: false
        onTriggered: left(0)
    }

    function leave() {
        timer.start()
    }
}
