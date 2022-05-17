import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import org.onvif.device 1.0
import org.onvif.common 1.0
import "helper.js" as Helper
import "controls" as Controls

Controls.ScrollablePage {

    id: settingsPage
    title: qsTr("Devices")

    onIsCurrentPageChanged: {

        if (settingsPage.isCurrentPage) {
            refreshDevices()
        } else {
            timer.stop()
            deviceDiscoveryModel.stop()
        }
    }

    function refreshDevices() {
        deviceDiscoveryModel.reset()
        deviceDiscoveryModel.start()
        timer.restart()
    }

    Timer {

        id: timer
        interval: 2000
        running: false
        repeat: false

        onTriggered: {
            deviceDiscoveryModel.stop()
        }
    }

    Connections {
        target: DeviceManager
        function onDeviceAdded() {
            deviceDiscoveryModelFiltered.reload()
        }
        function onDeviceRemoved() {
            deviceDiscoveryModelFiltered.reload()
        }
    }

    DeviceDiscoverModel {

        id: deviceDiscoveryModel
    }

    SortFilterProxyModel {

        id: deviceDiscoveryModelFiltered
        model: deviceDiscoveryModel
        sortOrder: Qt.AscendingOrder
        filterRole: Enums.IdRole
        filterFunctor: function (value) {
            return !DeviceManager.containsDevice(value)
        }
    }

    Controls.GridLayout {

        width: parent.width

        Controls.GroupBox {

            id: group
            title: qsTr("Available Devices")
            infoText: qsTr("All devices found on the network are listed here.")

            mainAction: Controls.BusyAction {
                busy: timer.running
                text: qsTr("Search again")
                onTriggered: {
                    settingsPage.refreshDevices()
                }
            }

            ColumnLayout {

                anchors.fill: parent
                anchors.leftMargin: -group.leftPadding
                anchors.rightMargin: -group.rightPadding

                DevicesListView {

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: deviceDiscoveryModelFiltered

                    onClicked: {

                        const modelData = deviceDiscoveryModelFiltered.get(
                                            index)

                        Helper.createDialog("dialogs/NewOnvifDeviceDialog.qml",
                                            ApplicationWindow.window, {
                                                "deviceName": modelData.name,
                                                "deviceEndpoint": modelData.endpoint,
                                                "deviceId": modelData.id,
                                                "deviceNameFixed": false,
                                                "deviceEndpointFixed": true
                                            })
                    }
                }

                Label {

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.leftMargin: group.leftPadding
                    Layout.rightMargin: group.rightPadding

                    text: qsTr("No devices found on your network. Please make sure you are on the same network as the device.")
                    wrapMode: Text.WordWrap

                    visible: !timer.running
                             && deviceDiscoveryModelFiltered.count === 0
                }
            }
        }

        Controls.GroupBox {

            id: group1
            title: qsTr("Configured Devices")

            ColumnLayout {

                anchors.fill: parent
                anchors.leftMargin: -group1.leftPadding
                anchors.rightMargin: -group1.rightPadding

                DevicesListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    model: DevicesModel {
                        deviceManager: DeviceManager
                    }

                    onClicked: {

                        const modelData = model.get(index)

                        Helper.createDialog("dialogs/NewOnvifDeviceDialog.qml",
                                            ApplicationWindow.window, {
                                                "deviceName": modelData.name,
                                                "deviceEndpoint": modelData.endpoint,
                                                "deviceId": modelData.id,
                                                "deviceNameFixed": false,
                                                "deviceEndpointFixed": true
                                            })
                    }
                }
            }
        }

        Controls.GroupBox {

            title: "test"

            Label {
                text: "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet."
                wrapMode: Text.WordWrap
                width: parent.width
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
}
