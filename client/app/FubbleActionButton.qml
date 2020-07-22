import QtQuick 2.12

Image {

    id: actionImage
    property int imageSize: 40
    property var actionFunction
    property alias toolTipText: actionToolTip.text
    property bool showToolTip: true
    signal actionClick

    sourceSize.height: imageSize
    sourceSize.width: imageSize

    FubbleToolTip {
        id: actionToolTip
        visible: maAction.containsMouse && showToolTip
    }

    MouseArea {
        id: maAction
        anchors.fill: parent
        hoverEnabled: true
        onClicked: actionClick()
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

