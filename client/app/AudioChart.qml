import QtQuick 2.14
import QtCharts 2.3
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import "."
import io.fubble 1.0
import "scripts/utils.js" as Utils

Rectangle {
    clip: true
    property int maxValues: 50
    property int countSilence: 0
    color: Style.current.transparent
    property alias chart: audioChart

    property var audioLevels: []

    function addNewAudioLevel(level) {
        if (audioLevels.length >= maxValues) {
            audioLevels.splice(0, 1)
        }

        if (level === 0) {
            countSilence++
        } else {
            countSilence = 0
        }

        if (countSilence < 50) {
            audioLevels.push(level)
            insertAudioLevels()
        }
    }

    ChartView {
        id: audioChart
        antialiasing: true
        clip: true
        legend.visible: false
        anchors.fill: parent
        backgroundColor: Style.current.transparent
        plotArea: Qt.rect(0, 0, width, height)

        margins {
            top: 0
            bottom: 0
            left: 0
            right: 0
        }

        ValueAxis {
            id: axisX
            min: 0
            max: maxValues
            labelsVisible: false
            gridVisible: false
            lineVisible: false
            color: Style.current.transparent
        }

        ValueAxis {
            id: axisY
            min: -5
            max: 127
            labelsVisible: false
            gridVisible: false
            lineVisible: false
            color: Style.current.transparent
        }

        AreaSeries {
            id: audioSeries
            color: Style.current.primary

            axisX: axisX
            axisY: axisY

            upperSeries: LineSeries {
                color: Style.current.transparent
            }
        }
    }

    Component.onCompleted: {
        if (Utils.isDemoMode()) {
            audioLevels = [40, 60, 80, 85, 80, 75, 75, 68, 72, 59, 80, 99, 117, 44, 36, 116, 48, 70, 77, 78, 112, 112, 127, 127, 59, 67, 45, 6, 4, 3, 1, 1, 0, 0, 0, 1, 5, 21, 30, 83, 60, 91, 17, 43, 125, 53, 50, 14, 94, 22]
        }
        insertAudioLevels()
    }

    function insertAudioLevels() {
        audioSeries.upperSeries.clear()

        for (var i = 0; i < audioLevels.length; i++) {
            var level = audioLevels[i]
            audioSeries.upperSeries.insert(i, i, level)
        }
    }
}
