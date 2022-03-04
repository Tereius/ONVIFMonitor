import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

GroupBox {

    id: control

    property string text: ""
    property bool alert: false

    label: Item {}

    ColumnLayout {

        width: parent.width
        spacing: 10

        RowLayout {
            height: 50
            Layout.fillWidth: true

            Icon {
                icon.name: control.alert ? "ic_warning" : "information"
                icon.color: control.alert ? Material.color(
                                                Material.Red) : Material.accentColor
            }

            T.Label {
                text: control.title
                Layout.fillWidth: true
            }
        }

        Rectangle {
            height: 2
            Layout.fillWidth: true
            color: control.Material.backgroundColor
        }

        T.Label {
            text: control.text
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }
    }
}
