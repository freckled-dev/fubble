import QtQuick 2.12

Image {
    id: questionImage
    source: Style.current.feedbackImage
    sourceSize.width: 25
    sourceSize.height: 25

    FubbleToolTip {
        visible: maQuestions.containsMouse
        width: 250
        text: qsTr("Feedback? Questions? Problems?\nFeel free to contact us anytime!")
    }

    MouseArea {
        id: maQuestions
        hoverEnabled: true
        anchors.fill: parent
        onClicked: openFeedbackForm()
    }

    function openFeedbackForm() {
        Qt.openUrlExternally("https://freckled.dev/contact/")
    }
}
