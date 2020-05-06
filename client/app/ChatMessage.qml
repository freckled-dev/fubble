import QtQuick 2.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "."

Rectangle {
    radius: 5
    border.width: 1
    implicitWidth: chatColumn.implicitWidth
    implicitHeight: chatColumn.implicitHeight

    ColumnLayout {
        id: chatColumn

        Label {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 10
            color: own ? Style.themeLight.accent : Style.themeLight.primary
            text: name + " - " + new Date(timestamp).toTimeString()
        }

        Label {
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.maximumWidth: element.width * 2 / 3
            Layout.rightMargin: 10
            text: message
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

