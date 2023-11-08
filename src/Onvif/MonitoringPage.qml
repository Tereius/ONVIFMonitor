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

    Rally.Popup {
        id: dialog
        title: "Add Monitor"

        property var index

        ColumnLayout {

            width: parent.width

            Repeater {
                model: devicesModel

                delegate: ItemDelegate {
                    text: name
                    icon.name: "camera-wireless"
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
            icon.name: "plus"
            text: qsTr("Add Page")
            onTriggered: monitorGridModel.addPage("Page")
            enabled: !monitoringPage.editable
        },

        Kirigami.Action {
            icon.name: "cog"
            text: qsTr("Edit")
            onTriggered: monitoringPage.editable = !monitoringPage.editable
        }
    ]*/
    Connections {
        target: monitoringPage.flickable
        function onContentXChanged() {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }

        function onContentYChanged() {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }

        function onHeightChanged() {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }

        function onWidthChanged() {
            Qt.callLater(monitoringPage.updateVisibleArea)
        }
    }

    function updateVisibleArea() {

        monitoringPage.visibleArea = Qt.rect(monitoringPage.flickable.contentX,
                                             monitoringPage.flickable.contentY,
                                             monitoringPage.flickable.width,
                                             monitoringPage.flickable.height)
    }


    /*
    Rally.GroupBox {

        id: group
        width: Math.min(parent.width, 300)
        height: Math.min(parent.height, 150)
        anchors.centerIn: parent
        padding: 10
        visible: bar.count === 0

        Column {

            spacing: 20
            anchors.centerIn: parent

            Rally.Icon {
                icon.name: "plus"
                icon.width: 36
                icon.height: 36
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rally.Button {
                text: "Add page"
                onClicked: {
                    monitorGridModel.addPage("Page")
                }
            }
        }
    }*/
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
                                        implicitWidth: grid.effectiveAtomicWidth

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
                                            profileId: Onvif.createProfileId(
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

                                                icon.name: "play"
                                                anchors.centerIn: parent
                                                visible: true

                                                onClicked: {
                                                    let component = Qt.createComponent(
                                                                      "CameraStream.qml")
                                                    let mediaPlayer = component.createObject(
                                                            snapshot, {
                                                                "profileId": Onvif.createProfileId(
                                                                                 deviceId, token),
                                                                "width": 200,
                                                                "height": 200
                                                            })
                                                    mediaPlayer.profileId = Onvif.createProfileId(
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
                                    icon.name: "plus"
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
