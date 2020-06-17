import QtQuick 2.0

Image {

    id: actionImage
    property int imageSize: 40
    property var actionFunction
    property alias toolTipText: actionToolTip.text
    signal actionClick

    sourceSize.height: imageSize
    sourceSize.width: imageSize

    FubbleToolTip {
        id: actionToolTip
        visible: maAction.containsMouse
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

