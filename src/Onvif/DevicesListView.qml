import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Onvif
import MaterialRally as Rally

Rally.ListView {

    id: control

    delegate: Rally.ItemDelegate {

        id: delegate
        showChevron: true

        GridLayout {

            id: column
            anchors.left: parent.left
            anchors.leftMargin: parent.leftPadding
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.bottomPadding
            anchors.right: parent.right
            anchors.rightMargin: parent.rightPadding
            anchors.top: parent.top
            anchors.topMargin: parent.topPadding

            columns: 4
            rows: 2

            Label {
                id: nameLabel
                Layout.column: 0
                Layout.row: 0
                Layout.fillWidth: true
                text: model.name
                elide: Text.ElideRight
            }

            Label {
                id: hostLabel
                Layout.column: 0
                Layout.row: 1
                Layout.fillWidth: true
                text: model.host
                elide: Text.ElideRight
                color: Material.secondaryTextColor
            }

            Rally.Icon {
                Layout.column: 1
                Layout.rowSpan: 2
                icon.name: "web-off"
                icon.color: Material.color(Material.Red)
                visible: model.initialized != undefined
                         && model.initialized != null
                         && model.initialized === false
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
        }
    }
}
