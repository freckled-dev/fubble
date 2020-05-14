import QtQml 2.0
import QtMultimedia 5.0

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

    property bool videoDisabled

    property var video: MediaPlayer {
        source: video_source
        autoPlay: true
    }
}
