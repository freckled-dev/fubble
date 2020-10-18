import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import QtMultimedia 5.14
import io.fubble 1.0
import QtQuick.Controls.Material 2.14
import "."

Rectangle {
    color: Style.current.transparent
    radius: 5
    property alias headerLabelText: headerLabel.text
    property alias infoLabelText: infoLabel.text
    border.color: Style.current.foreground
    border.width: 1
    Material.foreground: Style.current.foreground
    Layout.fillWidth: true

    ColumnLayout {
        id: noImageColumnLayout
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        spacing: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: parent.left

        Label {
            id: headerLabel
            font.pointSize: Style.current.subHeaderPointSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            font.bold: true
            wrapMode: Text.WordWrap
        }
        Label {
            id: infoLabel
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

