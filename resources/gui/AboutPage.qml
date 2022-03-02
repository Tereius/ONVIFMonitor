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
