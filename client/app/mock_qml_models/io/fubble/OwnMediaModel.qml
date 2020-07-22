import QtQuick 2.12
import QtMultimedia 5.12

QtObject {
    property bool muted
    property bool videoDisabled
    property bool deafed
    property bool videoAvailable: true
    property bool loopbackOwnVoice
    signal newAudioLevel(int level)

    property MediaPlayer video: MediaPlayer {
        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
        autoPlay: true
    }
}
