import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.12

T.GroupBox {

    id: control

    property T.Action mainAction: T.Action {}

    background: Rectangle {
        width: parent.width
        height: parent.height
        color: "#373740"
    }

    label: ToolBar {

        x: control.leftPadding
        y: 0 //control.topPadding / 2 - height / 2
        implicitHeight: visible ? control.Material.buttonHeight : 0
        width: control.availableWidth
        visible: control.title.length > 0

        RowLayout {

            id: row
            width: parent.width
            height: parent.height

            T.Label {
                text: control.title
                height: parent.height
                elide: Text.ElideRight
                Layout.fillWidth: true
                anchors.verticalCenter: parent.verticalCenter
            }

            ToolButton {
                action: control.mainAction
                visible: action.text.length > 0
                implicitHeight: parent.height
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            color: Material.background
            height: 1
            anchors.bottom: parent.bottom
        }
    }


    /*
    label: T.Label {
        x: control.leftPadding
        y: control.topPadding / 2 - height / 2
        width: control.availableWidth
        text: control.title
        elide: Text.ElideRight
        font.capitalization: Font.AllUppercase
        font.letterSpacing: 0.8
    }*/
}
