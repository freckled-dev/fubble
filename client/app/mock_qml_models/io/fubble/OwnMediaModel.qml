import QtQuick 2.0

QtObject {
    property bool muted
    property bool videoDisabled
    property bool loopbackOwnVoice
    signal newAudioLevel(int level)
}
