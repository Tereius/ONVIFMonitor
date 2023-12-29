import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Onvif
import MaterialRally as Rally

Rally.Popup {

    id: monitorDialog

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
    }

    Rally.ScrollView {

        id: scrollView
        anchors.fill: parent
        enableVerticalScrollBar: monitorDialog.opened

        SwipeView {

            id: swipeView
            interactive: false
            anchors.fill: parent
            contentWidth: availableWidth

            Item {

                width: swipeView.availableWidth
                implicitHeight: columnLayoutPage1.implicitHeight

                ColumnLayout {

                    id: columnLayoutPage1
                    spacing: 0
                    implicitWidth: Math.min(parent.width, 600)
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
                                               swipeView.incrementCurrentIndex()
                                           }
                            }
                        }
                    }
                }
            }

            Item {

                width: swipeView.availableWidth
                implicitHeight: columnLayoutPage2.implicitHeight

                ColumnLayout {

                    id: columnLayoutPage2
                    implicitWidth: Math.min(parent.width, 600)
                    anchors.horizontalCenter: parent.horizontalCenter

                    Rally.GroupBox {

                        Layout.fillWidth: true

                        ColumnLayout {

                            width: parent.width

                            Label {
                                text: "asdf"
                            }
                        }
                    }
                }
            }
        }
    }
}
