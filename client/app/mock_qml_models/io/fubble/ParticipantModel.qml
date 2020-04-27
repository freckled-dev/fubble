import QtQml 2.0
import QtMultimedia 5.0

QtObject {
    property string name
    property string video_source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
    property var video: MediaPlayer {
        source: video_source
        autoPlay: true
    }
}
