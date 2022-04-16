import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.12 as L

L.GridLayout {

    id: control

    readonly property real minWidth: 200

    columns: Math.max(1, width / 300)

    columnSpacing: 20
    rowSpacing: 20

    onChildrenChanged: {

        for (var i = 0; i < control.children.length; i++) {

            control.children[i].L.Layout.fillWidth = true
            control.children[i].L.Layout.fillHeight = true
            control.children[i].L.Layout.alignment = Qt.AlignVCenter | Qt.AlignTop
            control.children[i].L.Layout.minimumWidth = control.minWidth
            control.children[i].L.Layout.preferredWidth = control.minWidth
        }
    }
}
