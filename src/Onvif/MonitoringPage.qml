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
                                         "dialogs/EditMonitorDialog.qml"), {},
                                     mapToGlobal(x, y).y)
                    dialog.editActionClicked.connect(profileId => {
                                                         monitorGridModel.addTile(
                                                             monitorGridModel.index(
                                                                 0, 0),
                                                             profileId.getDeviceId(
                                                                 ))
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

                                            leftPadding: 4
                                            rightPadding: 4
                                            bottomPadding: 4
                                            property var deviceInfo: DeviceManager.getDeviceInfo(
                                                                         deviceId)

                                            mainAction: Rally.BusyAction {
                                                text: qsTr("delete")
                                                icon.name: "delete"
                                                onTriggered: {
                                                    monitorGridModel.removeTile(
                                                                contentModel.rootIndex,
                                                                contentModel.modelIndex(
                                                                    index))
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
                                                profileId: Onvif.createProfileId(
                                                               deviceId, token)
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
        anchors.bottomMargin: parent.height * 0.03
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
                             Qt.resolvedUrl("dialogs/EditMonitorDialog.qml"),
                             {}, mapToGlobal(x, y).y)
            dialog.editActionClicked.connect(profileId => {
                                                 monitorGridModel.addTile(
                                                     monitorGridModel.index(0,
                                                                            0),
                                                     profileId.getDeviceId())
                                             })
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }
}
