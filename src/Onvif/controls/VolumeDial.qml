import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Dial {
    id: control

    Label {
        anchors.centerIn: parent
        text: Math.round(control.value * 100.0) + "%"
    }
}
