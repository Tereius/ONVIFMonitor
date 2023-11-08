import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Onvif

Page {

    property var deviceId

    title: qsTr("Event service info")

    QtObject {

        id: d
        property var eventServiceInfo: DeviceManager.getDeviceInfo(
                                           deviceId).eventService
    }

    ListModel {

        id: deviceInfoModel

        dynamicRoles: true

        Component.onCompleted: {

            append({
                       "label": qsTr("Endpoint"),
                       "value": d.eventServiceInfo.serviceEndpoint
                   })
            append({
                       "label": qsTr("WS subsc. policy support"),
                       "value": d.eventServiceInfo.wSSubscriptionPolicySupport
                   })
            append({
                       "label": qsTr("WS pull point support"),
                       "value": d.eventServiceInfo.wSPullPointSupport
                   })
            append({
                       "label": qsTr("WS pausable subscription support"),
                       "value": d.eventServiceInfo.wSPausableSubscriptionManagerInterfaceSupport
                   })
            append({
                       "label": qsTr("Max. notification producers"),
                       "value": d.eventServiceInfo.maxNotificationProducers
                   })
            append({
                       "label": qsTr("Max. pull points"),
                       "value": d.eventServiceInfo.maxPullPoints
                   })
            append({
                       "label": qsTr("Persistentce"),
                       "value": d.eventServiceInfo.persistentNotificationStorage
                   })

            infoListView.model = deviceInfoModel
        }
    }

    ScrollView {

        anchors.fill: parent

        ListView {

            id: infoListView

            anchors.fill: parent

            delegate: ItemDelegate {

                width: parent.width

                onClicked: {
                    if (showMoreSource) {
                        window.push(showMoreSource, showMoreParam)
                    }
                }

                contentItem: Column {

                    Label {
                        width: parent.width
                        text: label
                    }
                    Text {
                        width: parent.width
                        text: value
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }
        }
    }
}
