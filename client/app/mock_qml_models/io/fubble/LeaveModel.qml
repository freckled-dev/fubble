import QtQuick 2.15
import QtQml 2.0

QtObject {
    id: leaveContainer

    signal left(int reason)

    property var timer: Timer {
        id: timer
        interval: 0
        running: false
        repeat: false
        onTriggered: left(0)
    }

    function leave() {
        timer.start()
    }
}
