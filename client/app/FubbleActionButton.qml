import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Button {

    id: actionButton
    property var actionFunction
    property alias toolTipText: actionToolTip.text
    property bool showToolTip: true
    property int buttonWidth: 40
    property int buttonHeight: 50

    signal actionClick
    icon.color: Style.current.transparent

    width: buttonWidth
    Layout.preferredWidth: buttonWidth
    implicitWidth: buttonWidth
    height: buttonHeight
    Layout.preferredHeight: buttonHeight
    implicitHeight: buttonHeight

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

