import QtQuick 2.15
import QtQuick.Controls 2.15

Label {
    wrapMode: Text.WrapAnywhere
    // linkColor property works with the StyledText textFormat, but not with RichText
    // the link color in RichText can be specified by including CSS style tags in the text
    linkColor: Style.current.primary
    onLinkActivated: Qt.openUrlExternally(link)


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
