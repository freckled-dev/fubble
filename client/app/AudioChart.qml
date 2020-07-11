import QtQuick 2.0
import QtCharts 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "."
import io.fubble 1.0
import "scripts/utils.js" as Utils

Rectangle {
    clip: true
    property int audioLevel
    property int maxValues: 50
    property bool talking: audioLevels.length > 0
    property int countSilence: 0
    color: Style.current.transparent
    property alias chart: audioChart

    property var audioLevels: []

    onAudioLevelChanged: {
        console.log("level: " + audioLevel)
        if (audioLevels.length >= maxValues) {
            audioLevels.splice(0, 1)
        }

        if (audioLevel === 0) {
            countSilence++
        } else {
            countSilence = 0
        }

        if (countSilence >= 50) {
            talking = false
            audioLevels = []
        } else {
            talking = true
        }

        audioLevels.push(audioLevel)
        insertAudioLevels()
    }

    ChartView {
        id: audioChart
        antialiasing: true
        clip: true
        legend.visible: false
        backgroundColor: Style.current.transparent

        margins {
            top: 0
            bottom: 0
            left: 0
            right: 0
        }

        ValueAxis {
            id: axisX
            min: 0
            max: maxValues + 5
            labelsVisible: false
            gridVisible: false
            lineVisible: false
            color: Style.current.transparent
        }

        ValueAxis {
            id: axisY
            min: -5
            max: 127 + 5
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
            audioLevels = [40, 60, 80, 85, 80, 75, 75, 68, 72, 59, 80, 99, 117, 44, 36, 116, 48, 70, 77, 78, 112, 112, 108, 80, 59, 67, 45, 6, 4, 3, 0, 0, 0, 0, 10, 15, 18, 21, 30, 83, 60, 91, 17, 43, 125, 53, 50, 14, 94, 22]
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
