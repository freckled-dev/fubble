import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import "."

Popup {
    id: aboutDialog
    modal: true
    focus: true
    x: (container.width - width) / 2
    y: container.height / 6
    width: 400
    height: 350
    padding: 0

    Rectangle {
        id: rectangle
        color: Style.current.transparent
        radius: 5
        border.width: 1
        border.color: Style.current.white
        anchors.fill: parent
    }

    Label {
        id: headerLabel
        anchors.top: parent.top
        text: qsTr("About")
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: Style.current.headerPointSize
    }

    Image {
        id: fubbleIcon
        width: 100
        anchors.top: headerLabel.bottom
        height: 100
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        source: Style.current.logoImage
        fillMode: Image.PreserveAspectFit
    }

    Item {
        id: nameAndVersionLabel
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: fubbleIcon.bottom
        implicitHeight: childrenRect.height

        Label {
            id: appNameLabel
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Fubble")
            horizontalAlignment: Text.AlignHCenter
        }

        SelectableLabel {
            text: appVersion
            anchors.top: appNameLabel.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Label {
        id: companyLabel
        anchors.top: nameAndVersionLabel.bottom
        text: qsTr("by Freckled OG")
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
    }

    LabelWithLink {
        id: websiteLabel
        anchors.top: companyLabel.bottom
        text: qsTr("<a href='https://freckled.dev/contact'>Contact us...</a>")
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:200;anchors_width:200;anchors_x:41;anchors_y:29}D{i:2;anchors_height:222}
D{i:3;anchors_height:100;anchors_width:100}
}
##^##*/

