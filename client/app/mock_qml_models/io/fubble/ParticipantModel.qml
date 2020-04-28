import QtQml 2.0
import QtMultimedia 5.0

QtObject {
    property string name
    property string video_source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
    property MediaPlayer video
    property bool videoAvailable: video != null


    /* MediaPlayer {
        source: video_source
        autoPlay: true
    }*/
}
