import QtQuick 2.10
import QtQml 2.12
import QtQuick.Controls 2.3
import org.onvif.device 1.0
import QtAV 1.7

Pane {

    property alias stremUrl: player.source

    background: Rectangle {

        anchors.fill: parent
        color: "#000000"
    }

    Rectangle {
        anchors.fill: parent
        VideoOutput2 {
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectFit
            source: player
            opengl: true
        }
        AVPlayer {
            id: player

            avFormatOptions: ({
                                  "rtsp_transport": "tcp"
                              })

            Component.onDestruction: {
                player.stop()
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {

                if (player.playbackState === MediaPlayer.PlayingState) {
                    player.stop()
                } else {
                    player.play()
                }
            }
        }
    }
}
