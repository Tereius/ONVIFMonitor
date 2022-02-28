import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0
import org.onvif.common 1.0
import org.kde.kirigami 2.14 as Kirigami
import "helper.js" as Helper
import "controls" as Controls

Kirigami.ScrollablePage {

    id: settingsPage
    title: qsTr("Devices")
    supportsRefreshing: true

    Component.onCompleted: {

        deviceDiscoveryModel.start()
        timer.start()
    }

    Component.onDestruction: {

        timer.stop()
        deviceDiscoveryModel.stop()
    }

    onRefreshingChanged: {
        if (refreshing) {
            refreshDevices()
        }
        refreshing = false
    }

    function refreshDevices() {
        deviceDiscoveryModel.reset()
        deviceDiscoveryModel.start()
        timer.start()
    }

    Timer {

        id: timer
        interval: 15000
        running: false
        repeat: false
    }

    Controls.Popup {
        id: dialog
        title: "Title"

        onAboutToShow: {
            dialogLoader.active = true
        }

        onClosed: {
            dialogLoader.active = false
        }

        Loader {
            id: dialogLoader
            anchors.fill: parent
        }
    }

    Column {

        ListView {

            height: contentHeight
            width: settingsPage.width

            interactive: false

            header: RowLayout {

                width: parent.width
                height: 50

                spacing: 10

                Label {

                    Layout.fillWidth: true

                    elide: Text.ElideRight
                    text: qsTr("Configured devices")
                    font.pixelSize: 15
                }

                Controls.Button {

                    text: qsTr("Add")
                    icon.name: "ic_add"
                    flat: true
                    onClicked: {

                        dialogLoader.setSource(Qt.resolvedUrl(
                                                   "NewDevicePage.qml"), {
                                                   "deviceName": qsTr("New Device"),
                                                   "deviceEndpoint": "",
                                                   "deviceNameFixed": false,
                                                   "deviceEndpointFixed": false
                                               })

                        dialogLoader.item.accepted.connect(function () {
                            dialog.close()
                        })

                        dialog.open()
                    }
                }
            }

            footer: Text {

                width: parent.width
                height: parent.count == 0 ? 50 : 0
                visible: parent.count == 0 ? true : false
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap

                text: qsTr("Currently there are no configured devices")
            }

            onCountChanged: {
                console.log("count changed" + count)
            }

            model: DevicesModel {
                deviceManager: DeviceManager
            }

            delegate: ItemDelegate {

                width: parent.width
                height: 50

                text: name

                onClicked: {

                    dialogLoader.setSource(Qt.resolvedUrl("DevicePage.qml"), {
                                               "deviceId": id,
                                               "deviceNameFixed": false,
                                               "deviceEndpointFixed": false
                                           })

                    dialogLoader.item.accepted.connect(function () {
                        dialog.close()
                    })

                    dialog.open()
                }
                rightPadding: settingsButton.width + errorButton.width

                ToolButton {
                    id: settingsButton
                    icon.name: "ic_settings"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: {
                        deviceSettingsDialog.deviceId = deviceId
                        deviceSettingsDialog.open()
                    }
                }
                ToolButton {
                    id: errorButton
                    visible: !initialized
                    icon.name: "ic_warning"
                    icon.color: "#FCE100"
                    anchors.right: settingsButton.left
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: {
                        root.showError(qsTr("Device initialization error"),
                                       error)
                    }
                }
            }
        }

        ListView {

            height: contentHeight
            width: settingsPage.width

            interactive: false

            header: RowLayout {

                width: parent.width
                height: 50

                spacing: 10

                Label {

                    Layout.fillWidth: true

                    elide: Text.ElideRight
                    text: qsTr("Available devices")
                    font.pixelSize: 15
                }

                Controls.Button {

                    id: refreshButton
                    text: "Search"
                    flat: true
                    icon.name: "ic_refresh"
                    visible: false
                    onClicked: {
                        visible = false
                        discoveryProgress.visible = true
                        settingsPage.refreshDevices()
                    }
                    Connections {
                        target: timer
                        onTriggered: {
                            refreshButton.visible = true
                            discoveryProgress.visible = false
                            deviceDiscoveryModel.stop()
                        }
                    }
                }

                ProgressBar {

                    anchors.top: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right

                    id: discoveryProgress
                    visible: true
                    indeterminate: true
                }
            }

            footer: Text {

                id: serchDevicesFooter

                width: parent.width
                height: parent.count == 0 ? 50 : 0

                visible: parent.count == 0

                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap

                text: qsTr("No devices were found")
            }

            onCountChanged: {
                if (count == 0) {

                    //serchDevicesFooter.visible = true
                    //footerItem.height = 50
                } else {

                    //serchDevicesFooter.visible = false
                    //footerItem.height = 0
                }
            }

            model: deviceDiscoveryModelFiltered
            delegate: ItemDelegate {

                width: parent.width
                height: 50

                icon.name: "ic_fiber_new"
                text: name
                onClicked: {

                    dialogLoader.setSource(Qt.resolvedUrl("NewDevicePage.qml"),
                                           {
                                               "deviceName": name,
                                               "deviceEndpoint": endpoint,
                                               "deviceId": id,
                                               "deviceNameFixed": false,
                                               "deviceEndpointFixed": true
                                           })
                    dialogLoader.item.accepted.connect(function () {
                        dialog.close()
                    })

                    dialog.open()
                }
            }
        }
    }

    Connections {

        target: root
        onCredentialsProvided: {
            DeviceManager.reinitializeDevice(deviceId)
        }
    }

    DeviceDiscoverModel {

        id: deviceDiscoveryModel
    }

    Connections {
        target: DeviceManager
        onDeviceAdded: {
            deviceDiscoveryModelFiltered.invalidate()
        }
        onDeviceRemoved: {
            deviceDiscoveryModelFiltered.invalidate()
        }
    }

    SortFilterProxyModel {

        id: deviceDiscoveryModelFiltered
        model: deviceDiscoveryModel
        filterRole: Enums.IdRole
        filterFunctor: function (value) {
            return !DeviceManager.containsDevice(value)
        }
    }

    DeviceSettingsDialog {

        id: deviceSettingsDialog
    }
}
