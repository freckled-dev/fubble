import QtQuick 2.14
import QtQuick.Controls 2.14
import "."

ToolTip {
    delay: 0
    timeout: 0
    background: Rectangle {
        anchors.fill: parent
        color: Style.current.gray300
        border.color: Style.current.gray400
        border.width: 1
        radius: 4
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

