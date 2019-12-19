import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

// import QtMultimedia 5.0
Item {
    id: element
    width: 400
    height: 400

    ColumnLayout {
        id: columnLayout
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        //        VideoOutput {
        //            id: videoOutput
        //        }
        TextField {
            id: name
            text: qsTr("")
            placeholderText: "Your Name"
        }

        Button {
            id: join
            text: qsTr("Join")
            Layout.fillWidth: true
        }
    }

    Connections {
        target: name
        onAccepted: print("clicked")
    }
}
