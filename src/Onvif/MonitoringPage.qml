import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models
import TilesGrid
import Onvif
import QtQuick.Controls.Material
import MaterialRally as Rally

SwipePage {

    id: monitoringPage

    padding: 0

    property bool editable: false
    property rect visibleArea: Qt.rect(0, 0, 0, 0)

    MonitorGridModel {
        id: monitorGridModel
        deviceManager: DeviceManager
    }

    DevicesModel {
        id: devicesModel
        deviceManager: DeviceManager
    }

    Component.onCompleted: {

        if (!monitorGridModel.hasIndex(0, 0)) {
            // Create a default page
            monitorGridModel.addPage("Default")
        }
    }

    contentItem: Loader {

        anchors.fill: parent
        sourceComponent: DeviceManager.count > 0 ? (monitorGridModel.monitorCount > 0 ? monitorComponent : placeholderComponentNoMonitor) : placeholderComponent
    }

    Component {

        id: placeholderComponent

        SwipePage {

            Placeholder {
                anchors.centerIn: parent
                showButton: true
                buttonText: qsTr("Settings")
                buttonIcon.name: "cog"
                icon.name: "cctv-off"
                text: qsTr("Seems like you don't have any configured devices. Go to settings and add at least one device. After that come back here.")
                onClicked: {
                    monitoringPage.setCurrentIndex(2)
                }
            }
        }
    }

    Component {

        id: placeholderComponentNoMonitor

        SwipePage {

            Placeholder {
                anchors.centerIn: parent
                showButton: true
                buttonText: qsTr("Add monitor")
                buttonIcon.name: "plus"
                icon.name: "monitor-off"
                text: qsTr("Seems like you don't have any monitors yet. You may want to add one.")
                onClicked: {
                    const dialog = Rally.Helper.createDialog(
                                     Qt.resolvedUrl(
                                         "dialogs/AddMonitorDialog.qml"), {},
                                     mapToGlobal(x, y).y)
                    dialog.editActionClicked.connect((profileId, settings) => {
                                                         monitorGridModel.addTile(
                                                             monitorGridModel.index(
                                                                 0, 0),
                                                             profileId.getDeviceId(
                                                                 ), settings)
                                                     })
                }
            }
        }
    }

    Component {

        id: monitorComponent

        ColumnLayout {

            width: parent.width

            TabBar {
                id: bar
                Layout.fillWidth: true

                visible: monitorGridModel.columnCount() > 1

                Repeater {
                    model: monitorGridModel
                    TabButton {
                        text: name
                        rightPadding: deleteButton.visible ? deleteButton.width : undefined
                        RoundButton {
                            id: deleteButton
                            //visible: monitoringPage.editable
                            icon.name: "close"
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked: {
                                monitorGridModel.removePage(deviceId)
                            }
                        }
                    }
                }
            }

            SwipeView {

                interactive: false
                Layout.fillWidth: true

                Repeater {
                    id: view

                    model: DelegateModel {

                        model: monitorGridModel

                        delegate: Loader {

                            active: SwipeView.isCurrentItem

                            sourceComponent: Rally.GridLayout {

                                id: page

                                width: parent.width
                                columnSpacing: 4

                                property var modelIndex: view.model.modelIndex(
                                                             index)
                                Repeater {

                                    model: DelegateModel {

                                        id: contentModel
                                        model: monitorGridModel
                                        rootIndex: view.model.modelIndex(index)

                                        delegate: Rally.GroupBox {

                                            id: tile
                                            leftPadding: 4
                                            rightPadding: 4
                                            bottomPadding: 4
                                            property var deviceInfo: DeviceManager.getDeviceInfo(
                                                                         deviceId)

                                            mainAction: Rally.BusyAction {
                                                text: qsTr("edit")
                                                icon.name: "pencil"
                                                onTriggered: {

                                                    const dialog = Rally.Helper.createDialog(
                                                                     Qt.resolvedUrl(
                                                                         "dialogs/EditMonitorDialog.qml"),
                                                                     {
                                                                         "profileId": model.profile,
                                                                         "settings": model.settings
                                                                     },
                                                                     mapToGlobal(
                                                                         x,
                                                                         y).y)
                                                    dialog.editActionClicked.connect(
                                                                (profileId, settings) => {
                                                                    monitorGridModel.editTile(
                                                                        contentModel.rootIndex,
                                                                        contentModel.modelIndex(
                                                                            index),
                                                                        settings)
                                                                })

                                                    dialog.deleteActionClicked.connect(
                                                                (profileId, settings) => {
                                                                    monitorGridModel.removeTile(
                                                                        contentModel.rootIndex,
                                                                        contentModel.modelIndex(
                                                                            index))
                                                                })
                                                }
                                            }

                                            title: DeviceManager.getName(
                                                       deviceId)

                                            CameraImage {
                                                id: snapshot
                                                opacity: 1
                                                autoReload: true
                                                autoReloadInterval: 5000
                                                width: parent.width
                                                implicitHeight: Math.max(
                                                                    width * 9 / 16,
                                                                    width * imageHeight / Math.max(
                                                                        imageWidth,
                                                                        1))
                                                profileId: model.profile

                                                states: [
                                                    State {
                                                        name: "normal"
                                                        ParentChange {
                                                            target: snapshot
                                                            parent: tile.contentItem
                                                            x: 0
                                                            y: 0
                                                        }
                                                    },
                                                    State {
                                                        name: "reparented"
                                                        ParentChange {
                                                            target: snapshot
                                                            parent: monitoringPage
                                                            x: 0
                                                            y: 0
                                                        }
                                                    }
                                                ]

                                                transitions: Transition {
                                                    ParentAnimation {
                                                        NumberAnimation {
                                                            properties: "x,y"
                                                            duration: 200
                                                        }
                                                    }
                                                }

                                                property var cameraStream: null

                                                TapHandler {
                                                    onTapped: {
                                                        if (snapshot.cameraStream) {
                                                            snapshot.state = "normal"
                                                            snapshot.cameraStream.destroy()
                                                        } else {
                                                            snapshot.cameraStream
                                                                    = Rally.Helper.createItem(
                                                                        Qt.resolvedUrl(
                                                                            "CameraStream.qml"),
                                                                        snapshot, {
                                                                            "settings": model.settings,
                                                                            "profileId": model.profile,
                                                                            "anchors.fill": snapshot
                                                                        })
                                                            snapshot.state = "reparented"
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    RoundButton {
        parent: Overlay.overlay
        visible: DeviceManager.count > 0
        opacity: monitoringPage.isCurrentPage ? 1 : 0
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: parent.width * 0.03
        anchors.rightMargin: parent.width * 0.03
        icon.name: "plus"
        width: 66
        height: 66
        icon.width: width / 2
        icon.height: height / 2
        Material.elevation: 3
        Material.background: Material.accent
        onClicked: {
            const dialog = Rally.Helper.createDialog(
                             Qt.resolvedUrl("dialogs/AddMonitorDialog.qml"),
                             {}, mapToGlobal(x, y).y)
            dialog.editActionClicked.connect((profileId, settings) => {
                                                 monitorGridModel.addTile(
                                                     monitorGridModel.index(0,
                                                                            0),
                                                     profileId.getDeviceId(),
                                                     settings)
                                             })
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }

    QtObject {
        id: priv
    }
}
