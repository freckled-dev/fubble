import QtQuick 2.12

import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import QtQuick.Layouts 1.12
import "."

Slider {
    id: volumeSlider
    property color sliderColor

    wheelEnabled: true
    to: 1
    snapMode: Slider.SnapAlways
    from: 0
    stepSize: 0.1

    background: Rectangle {
        x: volumeSlider.leftPadding
        y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: volumeSlider.availableWidth
        height: implicitHeight
        radius: 2
        color: Style.current.gray300

        Rectangle {
            width: volumeSlider.visualPosition * parent.width
            height: parent.height
            color: sliderColor
            radius: 2
        }
    }

    handle: Rectangle {
        x: volumeSlider.leftPadding + volumeSlider.visualPosition
           * (volumeSlider.availableWidth - width)
        y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
        implicitWidth: 20
        implicitHeight: 20
        radius: 13
        color: volumeSlider.pressed ? Style.current.gray200 : Style.current.gray100
        border.color: Style.current.gray300
    }
}
