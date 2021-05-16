import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0
import org.onvif.common 1.0
import QuickFuture 1.0
import org.kde.kirigami 2.14 as Kirigami

Kirigami.ScrollablePage {

    title: qsTr("Monitoring")

    ColumnLayout {
        TileLayout {

            id: tyleLayout
        }

        RowLayout {
            Tile {

                color: "red"
                opacity: 0.5
            }

            Tile {

                color: "green"
                opacity: 0.5
            }

            Tile {

                color: "blue"
                opacity: 0.5
                rowSpan: 2
            }
        }

        SpinBox {
            id: from
            editable: true
        }

        SpinBox {
            id: to
            editable: true
        }

        Button {

            text: "move"
            onClicked: {

                tyleLayout.moveTile(tyleLayout.tileAt(from.value), to.value)
                from.value = to.value
            }
        }
    }
}
