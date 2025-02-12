import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import Onvif
import MaterialRally as Rally

Pane {

    id: control
    readonly property int page: 0

    MonitorGridModel {
        id: monitorGridModel
        deviceManager: DeviceManager
    }

    SwipeView {
        id: s
        anchors.fill: parent

        Repeater {

            id: r

            model: DelegateModel {

                id: contentModel
                model: monitorGridModel

                Component.onCompleted: {
                    rootIndex = monitorGridModel.index(control.page, 0)
                }

                delegate: Loader {
                    active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                    sourceComponent: Component {

                        CameraImage {

                            Label {
                                text: "asdfasdf" + model.name
                            }

                            id: snapshot
                            autoReload: true
                            autoReloadInterval: 5000
                            //width: 300
                            //height: 300
                            profileId: model.profile
                        }
                    }
                }
            }
        }

        background: Rectangle {
            color: "black"
            anchors.fill: parent
        }
    }
}
