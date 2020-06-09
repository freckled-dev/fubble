import QtQuick 2.0
import QtMultimedia 5.0

QtObject {

    function startPreviews() {
        for (var i = 0; i < categories.length; i++) {
            var previews = categories[i].previews
            for (var j = 0; j < previews.length; j++) {
                var preview = previews[j]
                preview.player.play()
            }
        }
    }

    function stopPreviews() {}

    property list<QtObject> categories: [
        QtObject {
            property string name: qsTr("Screens")
            property list<QtObject> previews: [
                QtObject {
                    property string description: "Screen 1"
                    property var player: MediaPlayer {
                        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
                        autoLoad: false
                    }
                }
            ]
        },
        QtObject {
            property string name: qsTr("Application Windows")
            property list<QtObject> previews: [
                QtObject {
                    property string description: "Firefox"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
                    }
                },
                QtObject {
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                }
            ]
        }
    ]
}
