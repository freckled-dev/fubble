import QtQuick 2.12
import QtMultimedia 5.12

QtObject {

    property bool desktopSharingActive: true

    function startPreviews() {
        for (var categoryIndex = 0; categoryIndex < categories.length; categoryIndex++) {
            var previews = categories[categoryIndex].previews
            for (var previewIndex = 0; previewIndex < previews.length; previewIndex++) {
                var preview = previews[previewIndex]
                preview.player.play()
            }
        }
    }

    function stopPreviews() {
        for (var categoryIndex = 0; categoryIndex < categories.length; categoryIndex++) {
            var previews = categories[categoryIndex].previews
            for (var previewIndex = 0; previewIndex < previews.length; previewIndex++) {
                var preview = previews[previewIndex]
                preview.player.stop()
            }
        }
    }

    function shareDesktop(id) {
        console.log("User wants to share the following screen: " + id)
        // TODO - switch layout
    }

    function stopShareDesktop() {
        console.log("stopShareDesktop")
    }

    property list<QtObject> categories: [
        QtObject {
            property string name: qsTr("Screens")
            property list<QtObject> previews: [
                QtObject {
                    property int id: 1
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
                    property int id: 2
                    property string description: "Firefox"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Big_Buck_Bunny_alt.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
                    property string description: "VLC"
                    property var player: MediaPlayer {
                        autoLoad: false
                        source: "/home/tamrielle/Downloads/Sintel_webm_extract.webm"
                    }
                },
                QtObject {
                    property int id: 3
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
