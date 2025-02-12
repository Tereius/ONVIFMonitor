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
        sourceComponent: DeviceManager.count > 0 ? (monitorGridModel.monitorCount
                                                    > 0 ? monitorComponent : placeholderComponentNoMonitor) : placeholderComponent
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
                    const dialog = Rally.Helper.createDialog(Qt.resolvedUrl("dialogs/AddMonitorDialog.qml"), {},
                                                             mapToGlobal(x, y).y)
                    dialog.editActionClicked.connect((profileId, settings) => {
                                                         monitorGridModel.addTile(monitorGridModel.index(0, 0),
                                                                                  profileId.getDeviceId(), settings)
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

                                property var modelIndex: view.model.modelIndex(index)
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
                                            property var deviceInfo: DeviceManager.getDeviceInfo(deviceId)

                                            mainAction: Rally.BusyAction {
                                                text: qsTr("edit")
                                                icon.name: "pencil"
                                                onTriggered: {

                                                    const dialog = Rally.Helper.createDialog(
                                                                     Qt.resolvedUrl("dialogs/EditMonitorDialog.qml"), {
                                                                         "profileId": model.profile,
                                                                         "settings": model.settings
                                                                     }, mapToGlobal(x, y).y)
                                                    dialog.editActionClicked.connect((profileId, settings) => {
                                                                                         monitorGridModel.editTile(
                                                                                             contentModel.rootIndex,
                                                                                             contentModel.modelIndex(
                                                                                                 index), settings)
                                                                                     })

                                                    dialog.deleteActionClicked.connect((profileId, settings) => {
                                                                                           monitorGridModel.removeTile(
                                                                                               contentModel.rootIndex,
                                                                                               contentModel.modelIndex(
                                                                                                   index))
                                                                                       })
                                                }
                                            }

                                            title: DeviceManager.getName(deviceId)

                                            Item {

                                                id: placeholder
                                                width: parent.width
                                                implicitHeight: Math.max(width * 9 / 16,
                                                                         width * snapshot.imageHeight / Math.max(
                                                                             snapshot.imageWidth, 1))

                                                CameraImage {
                                                    id: snapshot
                                                    autoReload: true
                                                    autoReloadInterval: 5000
                                                    profileId: model.profile
                                                    settings: model.settings
                                                    width: placeholder.width
                                                    height: placeholder.height

                                                    states: [
                                                        State {
                                                            name: "normal"
                                                            ParentChange {
                                                                target: snapshot
                                                                parent: placeholder
                                                                x: 0
                                                                y: 0
                                                                width: placeholder.width
                                                                height: placeholder.height
                                                            }
                                                        },
                                                        State {
                                                            name: "fullscreen"
                                                            ParentChange {
                                                                target: snapshot
                                                                parent: swiper
                                                                x: 0
                                                                y: swiper.height / 2 - height / 2
                                                                width: snapshot.landscape ? swiper.width : Math.max(
                                                                                                swiper.height * 16 / 9,
                                                                                                swiper.height
                                                                                                * snapshot.imageWidth / Math.max(
                                                                                                    snapshot.imageHeight,
                                                                                                    1))
                                                                height: snapshot.portrait ? swiper.height : Math.max(
                                                                                                swiper.width * 9 / 16,
                                                                                                swiper.width
                                                                                                * snapshot.imageHeight / Math.max(
                                                                                                    snapshot.imageWidth,
                                                                                                    1))
                                                            }
                                                        }
                                                    ]

                                                    transitions: Transition {
                                                        ParentAnimation {

                                                            via: swiper

                                                            NumberAnimation {
                                                                properties: "x,y, width, height"
                                                                duration: 200
                                                                easing.type: Easing.OutQuad
                                                            }
                                                        }
                                                    }

                                                    property var cameraStream: null
                                                    property bool fullscreen: false

                                                    TapHandler {

                                                        gesturePolicy: TapHandler.ReleaseWithinBounds
                                                        parent: snapshot.state === "fullscreen" ? swiper : snapshot

                                                        onTapped: {
                                                            if (snapshot.state === "fullscreen") {
                                                                swiper.color = "transparent"
                                                                snapshot.cameraStream.z = -1000
                                                                snapshot.cameraStream.destroy()
                                                                snapshot.state = "normal"
                                                            } else {
                                                                snapshot.cameraStream = Rally.Helper.createItem(
                                                                            Qt.resolvedUrl("CameraStream.qml"),
                                                                            snapshot, {
                                                                                "profileId": model.profile,
                                                                                "settings": model.settings,
                                                                                "anchors.fill": snapshot,
                                                                                "z": -1
                                                                            })

                                                                snapshot.cameraStream.firstFrame.connect(() => {
                                                                                                             snapshot.cameraStream.z = 1
                                                                                                         })
                                                                snapshot.state = "fullscreen"
                                                                swiper.color = "black"
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
            const dialog = Rally.Helper.createDialog(Qt.resolvedUrl("dialogs/AddMonitorDialog.qml"), {},
                                                     mapToGlobal(x, y).y)
            dialog.editActionClicked.connect((profileId, settings) => {
                                                 monitorGridModel.addTile(monitorGridModel.index(0, 0),
                                                                          profileId.getDeviceId(), settings)
                                             })
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }

    Rectangle {

        id: swiper
        z: 1
        parent: Overlay.overlay
        anchors.fill: parent
        color: "transparent"

        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }
    }

    QtObject {
        id: priv
    }
}
