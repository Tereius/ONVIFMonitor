import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Onvif
import MaterialRally as Rally

ListView {

    id: control

    signal clicked(var index)

    implicitWidth: contentWidth
    implicitHeight: contentHeight
    interactive: false

    add: Transition {
        id: transition
        SequentialAnimation {
            PropertyAction {
                property: "opacity"
                value: 0
            }
            PropertyAction {
                property: "scale"
                value: 0.9
            }
            PauseAnimation {
                duration: transition.ViewTransition.index * 20
            }
            NumberAnimation {
                properties: "opacity, scale"
                to: 1.0
                duration: 200
                easing.type: Easing.OutQuad
            }
        }
    }

    displaced: Transition {
        id: transition1
        SequentialAnimation {
            PauseAnimation {
                duration: transition1.ViewTransition.index * 20
            }
            NumberAnimation {
                properties: "opacity, scale"
                to: 1.0
                duration: 200
                easing.type: Easing.OutQuad
            }
        }
    }

    delegate: ItemDelegate {

        id: delegate
        width: ListView.view.width
        height: column.implicitHeight + topPadding + bottomPadding
        bottomInset: 1

        onClicked: {
            control.clicked(index)
        }

        GridLayout {

            id: column
            x: parent.leftPadding
            y: parent.topPadding

            width: parent.width - parent.rightPadding - parent.leftPadding
            columns: 4
            rows: 2

            Label {
                id: nameLabel
                Layout.column: 0
                Layout.row: 0
                Layout.fillWidth: true
                text: name
                elide: Text.ElideRight
            }

            Label {
                id: hostLabel
                Layout.column: 0
                Layout.row: 1
                Layout.fillWidth: true
                text: host
                elide: Text.ElideRight
                color: Material.secondaryTextColor
            }

            Rally.Icon {
                Layout.column: 1
                Layout.rowSpan: 2
                icon.name: "web-off"
                icon.color: Material.color(Material.Red)
                visible: initialized != null && initialized === false
            }

            Row {
                Layout.column: 2
                Layout.rowSpan: 2

                //Layout.rightMargin: Math.max(
                //                        delegate.width / 2 - implicitWidth - 40,
                //                        0)
                Repeater {
                    id: iconRepeater
                    model: profiles
                    Rally.Icon {
                        width: visible ? implicitWidth : 0
                        opacity: 0.5
                        visible: {
                            if (icon.name.length > 0) {
                                for (var i = 0; i < iconRepeater.count; i++) {
                                    if (i !== index) {
                                        const otherIcon = iconRepeater.itemAt(i)
                                        if (otherIcon && otherIcon.visible
                                                && otherIcon.icon.name === icon.name) {
                                            return false
                                        }
                                    }
                                }
                                return true
                            } else {
                                return false
                            }
                        }
                        icon.color: Material.secondaryTextColor
                        icon.name: {
                            switch (modelData) {
                            case "s":
                            case "S":
                            case "Streaming":
                            case "streaming":
                            case "t":
                            case "T":
                                // streaming
                                return "cctv"
                            case "g":
                            case "G":
                                // edge storage and retrieval
                                return "harddisk"
                            case "m":
                            case "M":
                                // Metadata and events for analytics applications
                                return "motion-sensor"
                            case "d":
                            case "D":
                            case "c":
                            case "C":
                            case "a":
                            case "A":
                                // access control
                                return "lock-smart"
                            default:
                                return ""
                            }
                        }

                        ToolTip.delay: 1000
                        ToolTip.visible: ma.containsMouse
                        ToolTip.text: {
                            switch (icon.name) {
                            case "cctv":
                                return qsTr("Streaming")
                            case "harddisk":
                                return qsTr("Recording")
                            case "motion-sensor":
                                return qsTr("Metadata and events")
                            case "lock-smart":
                                return qsTr("Access control")
                            }
                        }

                        MouseArea {
                            id: ma
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton
                            hoverEnabled: true
                        }
                    }
                }
            }

            Rally.Icon {
                Layout.column: 3
                Layout.rowSpan: 2
                icon.name: "chevron-right"
            }
        }

        Rectangle {
            height: 1
            color: Material.backgroundColor
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: parent.leftPadding
            anchors.rightMargin: parent.rightPadding
        }
    }
}
