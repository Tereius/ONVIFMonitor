import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0
import org.onvif.common 1.0
import "helper.js" as Helper
import "controls" as Controls

Controls.ScrollablePage {

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
        interval: 5000
        running: false
        repeat: false
    }

    GridLayout {

        //spacing: 10
        width: parent.width
        columns: Math.max(1, width / 300)

        Controls.GroupBox {

            title: qsTr("Available Devices")
            mainAction: Action {
                enabled: !timer.running
                text: qsTr("Search again")
                onTriggered: {
                    settingsPage.refreshDevices()
                }
            }

            Layout.fillWidth: true

            ColumnLayout {

                width: parent.width

                ProgressBar {
                    indeterminate: true
                    opacity: timer.running ? 1 : 0
                    Layout.fillWidth: true
                    enabled: timer.running
                }

                Repeater {

                    model: deviceDiscoveryModelFiltered

                    ItemDelegate {

                        Layout.fillWidth: true
                        height: 50

                        icon.name: "ic_fiber_new"
                        text: name
                        onClicked: {

                            const dialog = Helper.createItem(
                                             Qt.resolvedUrl(
                                                 "dialogs/NewOnvifDeviceDialog.qml"),
                                             ApplicationWindow.window, {
                                                 "deviceName": name,
                                                 "deviceEndpoint": endpoint,
                                                 "deviceId": id,
                                                 "deviceNameFixed": false,
                                                 "deviceEndpointFixed": true
                                             })

                            dialog.openWithAnimOffset()
                            dialog.closed.connect(function () {
                                dialog.destroy()
                            })
                        }
                    }
                }

                RowLayout {

                    Layout.fillWidth: true
                    visible: !timer.running
                             && deviceDiscoveryModelFiltered.count === 0

                    Label {
                        text: qsTr("No devices found on your network. Please make sure you are on the same network as the device.")
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }
        }

        Controls.GroupBox {

            title: qsTr("Configured Devices")
            Layout.fillWidth: true

            ColumnLayout {

                width: parent.width

                Repeater {

                    model: DevicesModel {
                        deviceManager: DeviceManager
                    }

                    delegate: ItemDelegate {

                        Layout.fillWidth: true
                        height: 50

                        icon.name: "ic_fiber_new"
                        text: name
                        onClicked: {

                            const dialog = Helper.createItem(
                                             Qt.resolvedUrl(
                                                 "dialogs/NewOnvifDeviceDialog.qml"),
                                             ApplicationWindow.window, {
                                                 "deviceName": name,
                                                 "deviceEndpoint": endpoint,
                                                 "deviceId": id,
                                                 "deviceNameFixed": false,
                                                 "deviceEndpointFixed": true
                                             })

                            dialog.openWithAnimOffset()
                            dialog.closed.connect(function () {
                                dialog.destroy()
                            })
                        }
                    }
                }
            }
        }


        /*
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

                        const dialog = Helper.createItem(
                                         Qt.resolvedUrl(
                                             "dialogs/NewOnvifDeviceDialog.qml"),
                                         ApplicationWindow.window, {
                                             "deviceName": qsTr("New Device"),
                                         })


                        dialog.openWithAnimOffset()
                        dialog.closed.connect(function () {
                            dialog.destroy()
                        })
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

                    const dialog = Helper.createItem(
                                     Qt.resolvedUrl(
                                         "dialogs/NewOnvifDeviceDialog.qml"),
                                     null, {
                                         "deviceId": id,
                                         "deviceNameFixed": false,
                                         "deviceEndpointFixed": false
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

                    const dialog = Helper.createItem(
                                     Qt.resolvedUrl(
                                         "dialogs/NewOnvifDeviceDialog.qml"),
                                     ApplicationWindow.window, {
                                         "deviceName": name,
                                         "deviceEndpoint": endpoint,
                                         "deviceId": id,
                                         "deviceNameFixed": false,
                                         "deviceEndpointFixed": true
                                     })


                    dialog.openWithAnimOffset()
                    dialog.closed.connect(function () {
                        dialog.destroy()
                    })
                }
            }
        }
    */
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
