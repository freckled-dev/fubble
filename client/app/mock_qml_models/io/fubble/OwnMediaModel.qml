import QtQuick 2.15
import QtMultimedia 5.15

QtObject {
    property bool muted
    property bool videoDisabled
    property bool deafed
    property bool videoAvailable: true
    property bool loopbackOwnVoice
    signal newAudioLevel(int level)
    signal newAudioTestLevel(int level)

    property MediaPlayer video: MediaPlayer {
        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
        autoPlay: true
    }
}
