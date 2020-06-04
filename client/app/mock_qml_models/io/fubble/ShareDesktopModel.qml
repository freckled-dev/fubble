import QtQuick 2.0
import QtMultimedia 5.0

QtObject {

    property list<QtObject> categories: [
        QtObject {
            property string name: qsTr("Screens")
            property list<QtObject> previews: [
                QtObject {
                    property string description: "Screen 1"
                    property var preview: MediaPlayer {
                        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
                        autoPlay: true
                    }
                }
            ]
        },
        QtObject {
            property string name: qsTr("Application Windows")
            property list<QtObject> previews: [
                QtObject {
                    property string description: "Firefox"
                    property var preview: MediaPlayer {
                        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
                        autoPlay: true
                    }
                },
                QtObject {
                    property string description: "VLC"
                    property var preview: MediaPlayer {
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                        autoPlay: true
                    }
                }
            ]
        }
    ]
}
