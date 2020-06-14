import QtQuick 2.0
import QtCharts 2.14
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2

Rectangle {
    property var audioParticipant
    property int maxValues: 50

    Connections {
        target: audioParticipant
        onAudioLevelChanged: {
            if (audioParticipant.audioLevels.length >= maxValues) {
                audioParticipant.audioLevels.splice(0, 1)
            }

            audioParticipant.audioLevels.push(audioParticipant.audioLevel)
            insertAudioLevels()
        }
    }

    ChartView {
        id: audioChart
        anchors.fill: parent
        legend.visible: false
        antialiasing: true
        plotArea: Qt.rect(5, -5, width - 5, height - 5)

        ValueAxis {
            id: axisX
            min: 0
            max: maxValues
            labelsVisible: false
            gridVisible: false
            lineVisible: false
        }

        ValueAxis {
            id: axisY
            min: 0
            max: 127
            labelsVisible: false
            gridVisible: false
            lineVisible: false
        }

        SplineSeries {
            id: audioSeries

            axisX: axisX
            axisY: axisY
        }
    }

    Component.onCompleted: insertAudioLevels()

    function insertAudioLevels() {
        audioSeries.clear()

        for (var i = 0; i < audioParticipant.audioLevels.length; i++) {
            var level = audioParticipant.audioLevels[i]
            audioSeries.insert(i, i, level)
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

