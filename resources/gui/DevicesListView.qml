import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import org.onvif.common 1.0
import "./controls" as Controls

Column {

    id: control

    property alias model: repeater.model

    signal clicked(var data)

    Repeater {

        id: repeater

        anchors.fill: parent

        delegate: ItemDelegate {

            implicitWidth: rowLayout.implicitWidth + leftPadding + rightPadding
            implicitHeight: column.implicitHeight + topPadding + bottomPadding

            onClicked: {
                control.clicked(control.model.get(index))
            }

            Column {

                id: column
                x: parent.leftPadding
                y: parent.topPadding

                RowLayout {

                    id: rowLayout

                    Label {
                        text: name
                        Layout.fillWidth: true
                    }

                    Repeater {
                        model: services

                        delegate: Controls.Icon {

                            icon.name: {
                                switch (modelData) {
                                case DeviceProbe.ONVIF_EVENT:
                                    return "signal-distance-variant"
                                case DeviceProbe.ONVIF_MEDIA:
                                    return "cctv1"
                                case DeviceProbe.ONVIF_MEDIA2:
                                    return "cctv2"
                                case DeviceProbe.ONVIF_ANALYTICS:
                                    return "motion-sensor"
                                case DeviceProbe.ONVIF_DISPLAY:
                                    return "monitor"
                                case DeviceProbe.ONVIF_IMAGING:
                                    return "tune-vertical"
                                case DeviceProbe.ONVIF_PTZ:
                                    return "cursor-move"
                                case DeviceProbe.ONVIF_RECEIVER:
                                    return "audio-video"
                                case DeviceProbe.ONVIF_RECORDING:
                                    return "harddisk"
                                case DeviceProbe.ONVIF_REPLAY:
                                    return "play-pause"
                                default:
                                    return ""
                                }
                            }

                            visible: icon.name.length > 0

                            ToolTip.delay: 1000
                            ToolTip.visible: ma.containsMouse
                            ToolTip.text: {
                                switch (modelData) {
                                case DeviceProbe.ONVIF_EVENT:
                                    return qsTr("Event Service")
                                case DeviceProbe.ONVIF_MEDIA:
                                    return qsTr("Media Service Ver. 1")
                                case DeviceProbe.ONVIF_MEDIA2:
                                    return qsTr("Media Service Ver. 2")
                                case DeviceProbe.ONVIF_ANALYTICS:
                                    return qsTr("Analytics Service")
                                case DeviceProbe.ONVIF_DISPLAY:
                                    return qsTr("Display Service")
                                case DeviceProbe.ONVIF_IMAGING:
                                    return qsTr("Imaging Service")
                                case DeviceProbe.ONVIF_PTZ:
                                    return qsTr("PTZ Service")
                                case DeviceProbe.ONVIF_RECEIVER:
                                    return qsTr("Receiver Service")
                                case DeviceProbe.ONVIF_RECORDING:
                                    return qsTr("Recording Service")
                                case DeviceProbe.ONVIF_REPLAY:
                                    return qsTr("Replay Service")
                                default:
                                    return ""
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

                RowLayout {

                    Label {
                        text: manufacturer
                        Layout.fillWidth: true
                    }
                }
            }

            Rectangle {
                height: 1
                color: Material.backgroundColor
                width: parent.width
                anchors.bottom: parent.bottom
            }
        }
    }
}
