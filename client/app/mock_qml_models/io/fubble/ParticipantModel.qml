import QtQml 2.0
import QtMultimedia 5.0

QtObject {
    property string name
    property string video_source
    property bool own

    // self muted
    property bool muted
    // self muted
    property bool deafed
    // asdf
    property bool silenced
    // self muted
    property double volume: 1.0

    property bool videoDisabled

    property var video: MediaPlayer {
        source: video_source
        autoPlay: true
    }
}
