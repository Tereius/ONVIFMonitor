import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

T.GroupBox {

    id: control

    background: Rectangle {
        width: parent.width
        height: parent.height
        color: "#3a3a44"
    }

    label: T.Label {
        x: control.leftPadding
        y: control.topPadding / 2 - height / 2
        width: control.availableWidth
        text: control.title
        elide: Text.ElideRight
        font.capitalization: Font.AllUppercase
        font.letterSpacing: 0.8
    }
}
