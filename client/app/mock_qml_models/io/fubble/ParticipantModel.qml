import QtQml 2.0
import QtMultimedia 5.0

QtObject {
    property string name
    property string video_source
    property var video: MediaPlayer {
        source: video_source
        autoPlay: true
    }
}
