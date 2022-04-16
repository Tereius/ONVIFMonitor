import QtQuick 2.12
import QtQuick.Controls 2.12
import "controls" as Controls

Controls.ScrollablePage {

    id: aboutPage
    title: qsTr("About")

    Controls.GroupBox {
        title: qsTr("honorable mentions")

        Column {

            spacing: 10

            Label {
                text: "cctv, cctv-off Icon: Roberto Graham"
            }

            Label {
                text: "harddisk Icon: Austin Andrews"
            }

            Label {
                text: "playlist-remov Icon: Cody @XT3000"
            }

            Label {
                text: "signal-distance-variant Icon: https://materialdesignicons.com/icon/signal-distance-variant"
            }

            Label {
                text: "cursor-move Icon: https://materialdesignicons.com/icon/cursor-move"
            }

            Label {
                text: "audio-video Icon: https://materialdesignicons.com/icon/audio-video"
            }

            Label {
                text: "motion-sensor Icon: https://materialdesignicons.com/icon/motion-sensor"
            }

            Label {
                text: "play-pause Icon: https://materialdesignicons.com/icon/play-pause"
            }

            Label {
                text: "Material Icons: Google"
            }

            Label {
                text: "Material Studies Rally: Google"
            }

            Label {
                text: "Roboto Condensed: Google"
            }
        }
    }
}
