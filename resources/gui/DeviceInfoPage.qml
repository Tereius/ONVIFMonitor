import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0

Page {

    property var deviceId

    title: qsTr("Device info")

    QtObject {

        id: d
        property var deviceInfo: DeviceManager.getDeviceInfo(deviceId)
    }

    ListModel {

        id: deviceInfoModel

        dynamicRoles: true

        Component.onCompleted: {

            if (d.deviceInfo.error) {
                append({
                           "label": qsTr("Error"),
                           "value": d.deviceInfo.error
                       })
            }
            append({
                       "label": qsTr("Name"),
                       "value": d.deviceInfo.deviceName
                   })
            append({
                       "label": qsTr("Id"),
                       "value": d.deviceInfo.deviceId
                   })
            append({
                       "label": qsTr("Host"),
                       "value": d.deviceInfo.host
                   })
            append({
                       "label": qsTr("Port"),
                       "value": d.deviceInfo.port
                   })
            append({
                       "label": qsTr("Endpoint"),
                       "value": d.deviceInfo.deviceEndpoint
                   })
            append({
                       "label": qsTr("Endpoint id"),
                       "value": d.deviceInfo.endpointReference
                   })
            append({
                       "label": qsTr("Model"),
                       "value": d.deviceInfo.model
                   })
            append({
                       "label": qsTr("Manufacturer"),
                       "value": d.deviceInfo.manufacturer
                   })
            append({
                       "label": qsTr("Hardware"),
                       "value": d.deviceInfo.hardwareId
                   })
            append({
                       "label": qsTr("Firmware"),
                       "value": d.deviceInfo.firmwareVersion
                   })
            append({
                       "label": qsTr("Serial Nr"),
                       "value": d.deviceInfo.serialNumber
                   })
            append({
                       "label": qsTr("Time offset"),
                       "value": d.deviceInfo.dateTimeOffset
                   })
            append({
                       "label": qsTr("Event service"),
                       "value": qsTr("Show more"),
                       "showMoreSource": "EventServiceInfoPage.qml",
                       "showMoreParam": {
                           "deviceId": d.deviceInfo.deviceId
                       }
                   })
            append({
                       "label": qsTr("Media service"),
                       "value": qsTr("Show more"),
                       "showMoreSource": "MediaServiceInfoPage.qml",
                       "showMoreParam": {
                           "deviceId": d.deviceInfo.deviceId
                       }
                   })

            infoListView.model = deviceInfoModel
        }
    }

    ScrollView {

        anchors.fill: parent

        ListView {

            id: infoListView

            anchors.fill: parent

            //height: contentHeight
            //width: parent.width
            //model: deviceInfoModel
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

                rightPadding: moreButton.width

                Icon {
                    id: moreButton
                    name: "ic_chevron_right"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    visible: showMoreSource ? true : false
                }
            }
        }
    }
}
