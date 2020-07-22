import QtQml 2.0
import QtMultimedia 5.12

QtObject {
    property string name
    property string video_source
    property bool own

    // self muted
    property bool muted

    // all others muted
    property bool deafed

    // specific participant muted
    property bool silenced

    // volume setting from 0 to 1
    property double volume: 1.0

    // indicator if the participant is speaking
    property bool voiceDetected

    property bool videoDisabled

    // just used in the GUI
    property bool highlighted

    property var video: MediaPlayer {
        source: video_source
        autoPlay: true
    }

    // signal with new audio level value
    signal newAudioLevel(int level)
}
