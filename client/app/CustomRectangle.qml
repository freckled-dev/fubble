import QtQuick 2.12

Item {
    property alias color: innerRect.color

    property alias borderColor: borderRect.color
    property int borderWidth: 0

    property int lBorderwidth: borderWidth
    property int rBorderwidth: borderWidth
    property int tBorderwidth: borderWidth
    property int bBorderwidth: borderWidth

    Rectangle {
        id: borderRect
        anchors.fill: parent

        Rectangle {
            id: innerRect

            anchors {
                fill: parent

                leftMargin: lBorderwidth
                rightMargin: rBorderwidth
                topMargin: tBorderwidth
                bottomMargin: bBorderwidth
            }
        }
    }
}
