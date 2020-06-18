import QtQuick 2.0
import QtCharts 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "."
import io.fubble 1.0

Rectangle {
    clip: true
    property int audioLevel
    property int maxValues: 50
    property bool talking: audioLevels.length > 0
    property int countLowDezibel: 0
    color: Style.current.transparent
    property alias chart: audioChart

    property var audioLevels: []

    onAudioLevelChanged: {
        if (audioLevels.length >= maxValues) {
            audioLevels.splice(0, 1)
        }

        if (audioLevel < 5) {
            countLowDezibel++
        } else {
            countLowDezibel = 0
        }

        if (countLowDezibel >= 50) {
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
                XYPoint {
                    x: 0
                    y: 0
                }
            }
        }
    }

    Component.onCompleted: insertAudioLevels()

    function insertAudioLevels() {
        audioSeries.upperSeries.clear()

        for (var i = 0; i < audioLevels.length; i++) {
            var level = audioLevels[i]
            audioSeries.upperSeries.insert(i, i, level)
        }
    }
}
