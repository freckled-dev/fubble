import QtQuick 2.0

Image {

    id: actionImage
    property int imageSize: 40
    property var actionFunction
    property alias toolTipText: actionToolTip.text
    signal actionClick
    property bool showToolTip: true

    sourceSize.height: imageSize
    sourceSize.width: imageSize

    FubbleToolTip {
        id: actionToolTip
        visible: showToolTip && maAction.containsMouse
    }

    MouseArea {
        id: maAction
        anchors.fill: parent
        hoverEnabled: showToolTip
        onClicked: actionClick()
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

