import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Onvif

Page {

    property var deviceId

    title: qsTr("Event service info")

    QtObject {

        id: d
        property var mediaServiceInfo: DeviceManager.getDeviceInfo(
                                           deviceId).mediaService
    }

    ListModel {

        id: deviceInfoModel

        dynamicRoles: true

        Component.onCompleted: {

            append({
                       "label": qsTr("Endpoint"),
                       "value": d.mediaServiceInfo.serviceEndpoint
                   })
            append({
                       "label": qsTr("Snapshot URI"),
                       "value": d.mediaServiceInfo.snapshotUri
                   })
            append({
                       "label": qsTr("Rotation"),
                       "value": d.mediaServiceInfo.rotation
                   })
            append({
                       "label": qsTr("Video source mode"),
                       "value": d.mediaServiceInfo.videoSourceMode
                   })
            append({
                       "label": qsTr("OSD"),
                       "value": d.mediaServiceInfo.osd
                   })
            append({
                       "label": qsTr("Temporary OSD text"),
                       "value": d.mediaServiceInfo.temporaryOSDText
                   })
            append({
                       "label": qsTr("EXI compression"),
                       "value": d.mediaServiceInfo.eXICompression
                   })
            append({
                       "label": qsTr("Max number profiles"),
                       "value": d.mediaServiceInfo.maximumNumberOfProfiles
                   })
            append({
                       "label": qsTr("RTP multicast"),
                       "value": d.mediaServiceInfo.rTPMulticast
                   })
            append({
                       "label": qsTr("Rtp Tcp"),
                       "value": d.mediaServiceInfo.rTPTcp
                   })
            append({
                       "label": qsTr("Rtp Rtsp Tcp"),
                       "value": d.mediaServiceInfo.rTPRtspTcp
                   })
            append({
                       "label": qsTr("Non aggregate control"),
                       "value": d.mediaServiceInfo.nonAggregateControl
                   })
            append({
                       "label": qsTr("Non Rtsp streaming"),
                       "value": d.mediaServiceInfo.noRTSPStreaming
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
