import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import Onvif
import MaterialRally as Rally

Rally.Dialog {

    id: control

    property var profileId: null
    property MonitorSettings settings: MonitorSettings {}

    signal editActionClicked(var profileId, var settings)
    signal deleteActionClicked(var profileId, var settings)

    onBackButtonClicked: {
        control.close()
    }

    actions: [
        Rally.BusyAction {
            id: deleteAction
            text: qsTr("Delete")
            icon.name: "delete"
            onTriggered: {
                control.deleteActionClicked(
                    secondPageLoader.item.mediaProfile.profileId,
                    secondPageLoader.item.settings)
                control.close()
            }
        },

        Rally.BusyAction {
            id: addAction
            text: qsTr("Save")
            icon.name: "check"
            onTriggered: {
                control.editActionClicked(
                    secondPageLoader.item.mediaProfile.profileId,
                    secondPageLoader.item.settings)
                control.close()
            }
        }
    ]

    Rally.ScrollView {

        id: scrollView
        anchors.fill: parent
        enableVerticalScrollBar: control.opened

        Loader {
            id: secondPageLoader
            anchors.fill: parent
            active: true
            asynchronous: true
            visible: status == Loader.Ready
            Component.onCompleted: {
                if (active) {
                    setSource(Qt.resolvedUrl("../pages/EditMonitorPage.qml"), {
                                  "mediaProfile": DeviceManager.getMediaProfile(
                                                      control.profileId),
                                  "settings": control.settings
                              })
                } else {
                    source = ""
                }
            }
        }
    }
}
