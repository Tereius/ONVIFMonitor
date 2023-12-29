import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Onvif
import MaterialRally as Rally

Rally.ListView {

    id: control

    property var deviceId: null

    MediaProfilesModel {
        id: profilesModel
        deviceId: control.deviceId
    }

    model: profilesModel

    header: Component {

        Item {

            anchors.horizontalCenter: parent.horizontalCenter
            visible: !DeviceManager.getDeviceInfo(control.deviceId).initialized
            implicitHeight: visible ? offlineIcon.implicitHeight : 0
            width: parent.width

            Rally.IconLabel {
                id: offlineIcon
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                icon.name: "web-off"
                icon.color: Material.color(Material.Red)
                text: qsTr("Device not initialized")
                display: "TextUnderIcon"
            }
        }
    }

    delegate: Rally.ItemDelegate {

        showChevron: true
        height: Math.max(snapshot.height, 80)

        CameraImage {

            id: snapshot
            profileId: model.profileId
            height: width * 9 / 16
            width: parent.width * 1 / 3
            anchors.verticalCenter: parent.verticalCenter
        }

        GridLayout {

            columns: 4
            rows: 3

            anchors.left: snapshot.right
            anchors.leftMargin: parent.leftPadding
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.bottomPadding
            anchors.top: parent.top
            anchors.topMargin: parent.topPadding
            anchors.right: parent.right
            anchors.rightMargin: parent.rightPadding

            Label {
                Layout.column: 0
                Layout.row: 0
                text: qsTr("Name:")
                elide: Text.ElideRight
            }

            Label {
                Layout.column: 0
                Layout.row: 1
                text: qsTr("Snapshots:")
                elide: Text.ElideRight
                color: Material.secondaryTextColor
            }

            Label {
                Layout.column: 0
                Layout.row: 2
                text: qsTr("Backchannel:")
                elide: Text.ElideRight
                color: Material.secondaryTextColor
            }

            Label {
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: true
                text: model.name
                elide: Text.ElideRight
            }

            Rally.Icon {
                Layout.column: 1
                Layout.row: 1
                Layout.fillWidth: true
                icon.width: 20
                icon.height: 20
                icon.name: model.hasSnapshot ? "check" : "close"
                icon.color: model.hasSnapshot ? Material.color(
                                                    Material.Green) : Material.color(
                                                    Material.Red)
            }

            Rally.Icon {
                Layout.column: 1
                Layout.row: 2
                Layout.fillWidth: true
                icon.width: 20
                icon.height: 20
                icon.name: model.hasBackchannel ? "check" : "close"
                icon.color: model.hasBackchannel ? Material.color(
                                                       Material.Green) : Material.color(
                                                       Material.Red)
            }
        }
    }
}
