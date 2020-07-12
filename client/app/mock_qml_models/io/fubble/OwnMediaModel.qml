import QtQuick 2.0

QtObject {
    property bool muted
    property bool videoDisabled
    signal newAudioLevel(int level)
}
