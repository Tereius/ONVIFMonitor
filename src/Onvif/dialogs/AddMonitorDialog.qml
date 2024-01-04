import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import Onvif
import MaterialRally as Rally

Rally.Dialog {

    id: monitorDialog

    signal editActionClicked(var profileId, var settings)

    title: qsTr("Select stream profile")

    onBackButtonClicked: {

        if (swipeView.currentIndex === 0) {
            monitorDialog.close()
        } else {
            swipeView.decrementCurrentIndex()
        }
    }

    actions: {
        if (swipeView.currentIndex === 1) {
            return [addAction]
        }
        return []
    }

    Rally.BusyAction {
        id: addAction
        text: qsTr("Add")
        icon.name: "check"
        onTriggered: {
            monitorDialog.editActionClicked(
                        secondPageLoader.item.mediaProfile.profileId,
                        secondPageLoader.item.settings)
            monitorDialog.close()
        }
    }

    Component {

        id: firstPage

        Item {

            width: swipeView.availableWidth
            implicitHeight: columnLayoutPage1.implicitHeight

            ColumnLayout {

                id: columnLayoutPage1
                spacing: 0
                width: Math.min(parent.width, 600)
                anchors.horizontalCenter: parent.horizontalCenter

                DevicesModel {
                    id: devicesModel
                    deviceManager: DeviceManager
                }

                Repeater {

                    model: devicesModel

                    Rally.GroupBox {

                        Layout.fillWidth: true
                        title: name
                        icon.name: "cctv"

                        MediaProfilesListView {

                            deviceId: id
                            width: parent.width

                            onClicked: index => {
                                           priv.profileId = model.get(
                                               index).profileId
                                           swipeView.incrementCurrentIndex()
                                       }
                        }
                    }
                }
            }
        }
    }

    Rally.ScrollView {

        id: scrollView
        anchors.fill: parent
        enableVerticalScrollBar: monitorDialog.opened

        SwipeView {

            id: swipeView
            width: monitorDialog.width
            interactive: false

            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isPreviousItem
                asynchronous: true
                sourceComponent: firstPage
                visible: status == Loader.Ready
            }

            Loader {
                id: secondPageLoader
                active: SwipeView.isCurrentItem
                asynchronous: true
                visible: status == Loader.Ready
                onActiveChanged: {
                    if (active) {
                        setSource(Qt.resolvedUrl(
                                      "../pages/EditMonitorPage.qml"), {
                                      "mediaProfile": DeviceManager.getMediaProfile(
                                                          priv.profileId)
                                  })
                    } else {
                        source = ""
                    }
                }
            }
        }
    }

    QtObject {
        id: priv
        property var profileId: null
    }
}
