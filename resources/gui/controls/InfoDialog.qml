import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.12

T.Dialog {

    id: control

    property alias text: label.text

    parent: T.Overlay.overlay
    modal: true
    focus: true
    anchors.centerIn: parent
    width: Math.min(parent.width / 2, 300)

    closePolicy: Popup.CloseOnEscape

    T.Overlay.modal: Item {

        Behavior on opacity {
            NumberAnimation {
                duration: 120
            }
        }

        Rectangle {
            anchors.fill: parent
            color: control.Material.backgroundColor
        }

        FastBlur {
            id: headerBlur
            width: T.ApplicationWindow.window ? T.ApplicationWindow.window.header.width : 0
            height: T.ApplicationWindow.window ? T.ApplicationWindow.window.header.height : 0
            source: T.ApplicationWindow.window ? T.ApplicationWindow.window.header : null
            radius: 32
            transparentBorder: true
        }

        FastBlur {
            id: mainBlur
            width: T.ApplicationWindow.window ? T.ApplicationWindow.window.contentItem.width : 0
            height: T.ApplicationWindow.window ? T.ApplicationWindow.window.contentItem.height : 0
            source: T.ApplicationWindow.window ? T.ApplicationWindow.window.contentItem : null
            radius: 32
            transparentBorder: true
            anchors.top: headerBlur.bottom
        }

        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.1
        }
    }

    enter: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                duration: 180
                easing.type: Easing.OutQuart
                from: 0
                to: 1.0
            }
            NumberAnimation {
                property: "scale"
                duration: 180
                easing.type: Easing.OutBack
                from: 0.8
                to: 1.0
            }
        }
    }

    exit: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                duration: 180
                easing.type: Easing.InQuart
                from: 1.0
                to: 0
            }
            NumberAnimation {
                property: "scale"
                duration: 180
                easing.type: Easing.InBack
                from: 1.0
                to: 0.8
            }
        }
    }

    contentItem: T.Label {
        id: label
        font.pixelSize: 16
        wrapMode: Text.WordWrap
    }

    background: Rectangle {
        color: "black"
    }

    footer: ToolButton {
        text: qsTr("Dismiss")
        onClicked: control.close()
        anchors.left: parent.left
        anchors.right: parent.right

        Rectangle {
            anchors.bottom: parent.top
            x: control.leftPadding - control.padding / 2
            color: control.Material.dividerColor
            height: 2
            width: control.availableWidth + control.padding / 2
        }
    }
}
