import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import TilesGrid 1.0
import org.onvif.device 1.0
import org.onvif.monitoring 1.0
import QtQuick.Controls.Material 2.12
import "controls" as Controls

Controls.ScrollablePage {

    id: monitoringPage
    title: qsTr("Monitoring")

    padding: 0

    property bool editable: false
    property rect visibleArea: Qt.rect(0, 0, 0, 0)

    Controls.Popup {
        id: dialog
        title: "Add Monitor"

        property var index

        ColumnLayout {

            width: parent.width

            Repeater {
                model: devicesModel

                delegate: ItemDelegate {
                    text: name
                    icon.name: "ic_linked_camera"
                    Layout.fillWidth: true
                    onClicked: {
                        monitorGridModel.addTile(dialog.index, id)
                        dialog.close()
                    }
                }
            }
        }
    }


    /*
    actions.contextualActions: [

        Kirigami.Action {
            icon.name: "ic_add"
            text: qsTr("Add Page")
            onTriggered: monitorGridModel.addPage("Page")
            enabled: !monitoringPage.editable
        },

        Kirigami.Action {
            icon.name: "ic_settings"
            text: qsTr("Edit")
            onTriggered: monitoringPage.editable = !monitoringPage.editable
        }
    ]*/
    Connections {
        target: monitoringPage.flickable
        onContentXChanged: {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }

        onContentYChanged: {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }

        onHeightChanged: {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }

        onWidthChanged: {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }
    }

    function updateVisibleArea() {

        monitoringPage.visibleArea = Qt.rect(monitoringPage.flickable.contentX,
                                             monitoringPage.flickable.contentY,
                                             monitoringPage.flickable.width,
                                             monitoringPage.flickable.height)
    }

    Controls.GroupBox {

        id: group
        width: Math.min(parent.width, 300)
        height: Math.min(parent.height, 150)
        anchors.centerIn: parent
        padding: 10
        visible: bar.count === 0

        Column {

            spacing: 20
            anchors.centerIn: parent

            Controls.Icon {
                icon.name: "ic_add"
                icon.width: 36
                icon.height: 36
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Controls.Button {
                text: "Add page"
                onClicked: {
                    monitorGridModel.addPage("Page")
                }
            }
        }
    }

    MonitorGridModel {
        id: monitorGridModel
        deviceManager: DeviceManager
    }

    DevicesModel {
        id: devicesModel
        deviceManager: DeviceManager
    }

    Column {

        padding: 0
        spacing: 0

        TabBar {
            id: bar
            width: parent.width
            Repeater {
                model: monitorGridModel
                TabButton {
                    text: name
                    rightPadding: deleteButton.visible ? deleteButton.width : undefined
                    RoundButton {
                        id: deleteButton
                        visible: monitoringPage.editable
                        icon.name: "ic_close"
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
            currentIndex: bar.currentIndex
            clip: true
            width: monitoringPage.width

            Repeater {
                id: view

                model: DelegateModel {
                    model: monitorGridModel

                    delegate: Loader {

                        active: SwipeView.isCurrentItem

                        sourceComponent: ColumnLayout {

                            id: page
                            property var modelIndex: view.model.modelIndex(
                                                         index)
                            width: parent.width
                            spacing: 10

                            FixedSizeGridLayout {

                                id: grid

                                onMoveIndex: function (fromIndex, toIndex) {

                                    monitorGridModel.move(
                                                view.model.modelIndex(index),
                                                fromIndex, toIndex)
                                }

                                model: DelegateModel {
                                    model: monitorGridModel
                                    rootIndex: view.model.modelIndex(index)

                                    delegate: Tile {

                                        id: tile

                                        draggable: monitoringPage.editable
                                        implicitWidth: grid.effectiveAtomicWidth + Number.EPSILON // On Linux there seems to be a bug. If Number.EPSILON is missing some elements are invisible

                                        onImplicitWidthChanged: {

                                            Qt.callLater(function () {
                                                implicitHeight = implicitWidth * 9 / 16
                                            })
                                        }

                                        columnSpan: 1
                                        rowSpan: 1

                                        CameraImage {
                                            id: snapshot
                                            opacity: 1
                                            anchors.fill: parent
                                            autoReload: true
                                            autoReloadInterval: 5000
                                            profileId: App.createProfileId(
                                                           deviceId, token)

                                            property var globalRecT: {
                                                tile.mapToItem(
                                                            monitoringPage.flickable,
                                                            0, 0, tile.width,
                                                            tile.height)
                                            }

                                            visible: {
                                                var gloablRec = Qt.rect(
                                                            tile.x, tile.y,
                                                            tile.width,
                                                            tile.height)

                                                return gloablRec.y + gloablRec.height
                                                        >= monitoringPage.visibleArea.y
                                                        && gloablRec.y
                                                        <= monitoringPage.visibleArea.y
                                                        + monitoringPage.visibleArea.height
                                            }

                                            Label {

                                                text: "y " + tile.y + " h " + tile.height
                                            }

                                            WRoundButton {

                                                icon.name: "ic_play_arrow"
                                                anchors.centerIn: parent
                                                visible: true

                                                onClicked: {
                                                    let component = Qt.createComponent(
                                                                      "CameraStream.qml")
                                                    let mediaPlayer = component.createObject(
                                                            snapshot, {
                                                                "profileId": App.createProfileId(
                                                                                 deviceId, token),
                                                                "width": 200,
                                                                "height": 200
                                                            })
                                                    mediaPlayer.profileId = App.createProfileId(
                                                                deviceId, token)
                                                    mediaPlayer.anchors.fill = snapshot
                                                }
                                            }
                                        }
                                    }
                                }

                                //anchors.left: parent.left
                                //anchors.right: parent.right
                                Layout.fillWidth: true
                                columns: Math.min(Math.max(width / 200, 1),
                                                  grid.count)

                                //spacing: 0
                                //rowSpacing: 0
                                //columnSpacing: 0
                                atomicWidth: 80
                                atomicHeight: 80


                                /*
                            background: Rectangle {
                                color: "grey"
                            }*/
                            }

                            Row {

                                spacing: 20

                                Switch {

                                    onCheckedChanged: {
                                        monitoringPage.editable = checked
                                    }
                                }

                                RoundButton {
                                    icon.name: "ic_add"
                                    width: 66
                                    height: 66
                                    icon.width: 33
                                    icon.height: 33
                                    Material.elevation: 2
                                    Material.background: Material.accent
                                    onClicked: {
                                        dialog.index = page.modelIndex
                                        dialog.open()
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
