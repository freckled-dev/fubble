import QtQuick 2.14
import QtQuick.Controls 2.14

Button {

    id: actionButton
    property var actionFunction
    property alias toolTipText: actionToolTip.text
    property bool showToolTip: true
    signal actionClick
    icon.color: Style.current.transparent
    width: 40
    implicitWidth: 40

    FubbleToolTip {
        id: actionToolTip
        visible: actionButton.hovered && showToolTip
    }

    onClicked: actionClick()
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

