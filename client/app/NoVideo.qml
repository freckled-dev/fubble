import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtMultimedia 5.0
import io.fubble 1.0
import QtQuick.Controls.Material 2.0
import "."

Rectangle {
    color: Style.current.transparent
    radius: 5
    border.color: Style.current.foreground
    border.width: 1
    Material.foreground: Style.current.foreground
    Layout.fillWidth: true

    ColumnLayout {
        id: noImageColumnLayout
        spacing: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: parent.left

        Label {
            text: qsTr("No video camera found.")
            font.pointSize: Style.current.subHeaderPointSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            font.bold: true
            wrapMode: Text.WordWrap
        }
        Label {
            text: qsTr("Please check your video camera and settings and restart the app or continue without video...")
            font.pointSize: Style.current.textPointSize
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

