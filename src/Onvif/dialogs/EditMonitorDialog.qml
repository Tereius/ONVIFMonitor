import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import Onvif
import MaterialRally as Rally

Rally.Popup {

    id: monitorDialog

    property var monitorId: Onvif.createUuidString()

    signal editActionClicked(var profileId)

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
            swipeView.currentItem.item.save()
            monitorDialog.editActionClicked(priv.mediaProfile.profileId)
            monitorDialog.close()
        }
    }

    ListModel {

        id: mainMenuModel

        ListElement {
            title: qsTr("Monitoring")
            component: ""
        }

        ListElement {
            title: qsTr("Recording")
            component: ""
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
                                           priv.mediaProfile = model.get(index)
                                           priv.deviceInfo = DeviceManager.getDeviceInfo(
                                               priv.mediaProfile ? priv.mediaProfile.profileId.getDeviceId(
                                                                       ) : "")
                                           swipeView.incrementCurrentIndex()
                                       }
                        }
                    }
                }
            }
        }
    }

    Component {

        id: secondPage

        Item {

            width: swipeView.availableWidth
            implicitHeight: columnLayoutPage2.implicitHeight

            Settings {
                id: settings
                category: "monitor_settings"
            }

            function save() {

                settings.setValue(
                            monitorDialog.monitorId + "/enableBackchannel",
                            backchannelStreamSwitch.checked)
                settings.setValue(monitorDialog.monitorId + "/audioCodec",
                                  codecs.currentValue.id)
                settings.setValue(
                            monitorDialog.monitorId + "/audioInputDevice",
                            audioInput.currentValue.id)
                settings.setValue(monitorDialog.monitorId + "/pushToTalk",
                                  pushToTalkSwitch.checked)
                settings.setValue(monitorDialog.monitorId + "/micSensitivity",
                                  sensitivityDial.value)
            }

            ColumnLayout {

                id: columnLayoutPage2
                width: Math.min(parent.width, 600)
                anchors.horizontalCenter: parent.horizontalCenter

                CameraStream {

                    profileId: priv.mediaProfile.profileId
                    Layout.fillWidth: true
                    implicitHeight: width * videoHeight / videoWidth
                }

                Rally.GroupBox {

                    Layout.fillWidth: true
                    title: qsTr("Video stream")
                    icon.name: "cctv"
                    mainAction: Rally.BusyAction {

                        id: videoStreamSwitch
                        checkable: true
                        checked: settings.value(
                                     monitorDialog.monitorId + "/enableVideoStream",
                                     true)
                    }

                    Rally.FormLayout {

                        width: parent.width

                        Label {
                            text: qsTr("Mirror")
                        }

                        RowLayout {

                            Button {
                                flat: true
                                icon.name: "flip-horizontal"
                                checkable: true
                                display: AbstractButton.IconOnly
                                scale: checked ? -1 : 1
                            }

                            Button {
                                flat: true
                                icon.name: "flip-vertical"
                                checkable: true
                                display: AbstractButton.IconOnly
                                scale: checked ? -1 : 1
                            }
                        }

                        Label {
                            text: qsTr("Rotate")
                        }

                        SpinBox {
                            from: 0
                            to: 359
                            editable: true
                        }
                    }
                }

                Rally.GroupBox {

                    Layout.fillWidth: true
                    title: qsTr("Audio stream")
                    icon.name: "speaker"
                    mainAction: Rally.BusyAction {

                        id: audioStreamSwitch
                        checkable: true
                        checked: settings.value(
                                     monitorDialog.monitorId + "/enableAudioStream",
                                     true)
                    }

                    Rally.FormLayout {

                        width: parent.width

                        Label {
                            text: qsTr("Volume")
                        }

                        Row {
                            Dial {
                                id: volume
                                value: settings.value(
                                           monitorDialog.monitorId + "/volume",
                                           1.0)
                            }
                        }
                    }
                }

                Rally.GroupBox {

                    Layout.fillWidth: true
                    title: qsTr("Audio backchannel")
                    icon.name: "bullhorn"
                    enabled: priv.mediaProfile.hasBackchannel
                    mainAction: Rally.BusyAction {

                        id: backchannelStreamSwitch
                        checkable: true
                        checked: settings.value(
                                     monitorDialog.monitorId + "/enableBackchannel",
                                     false)

                        onCheckedChanged: {
                            if (checked) {
                                rtpSource.start(
                                            priv.mediaProfile.backchannelUrl)
                            } else {
                                rtpSource.stop()
                            }
                        }
                    }

                    MicrophoneRtpSource {

                        id: rtpSource
                        payloadFormat: MicrophoneRtpSource.RTP_PCMU_8000_1
                        audioInput: AudioInput {
                            device: audioInput.currentValue
                            volume: sensitivityDial.value
                            muted: talkButton.checkable ? !talkButton.checked : !talkButton.down
                        }
                    }

                    Rally.FormLayout {

                        width: parent.width

                        Label {
                            text: qsTr("Codec")
                        }

                        Rally.ComboBox {

                            id: codecs
                            currentIndex: 0
                            textRole: "codec"
                            Component.onCompleted: {
                                const codecId = settings.value(
                                                  monitorDialog.monitorId + "/audioCodec",
                                                  "Auto")
                                if (codecId.length > 0) {
                                    audioInput.currentIndex = Math.max(
                                                audioInput.find(codecId), 0)
                                } else {
                                    audioInput.currentIndex = 0
                                }
                            }
                            model: {
                                let encoder = [{
                                                   "id": "Auto",
                                                   "codec": "Auto"
                                               }]
                                const supportedEncoder = rtpSource.supportedEncoder(
                                                           priv.mediaProfile.backchannelSdp)
                                for (var i = 0; i < supportedEncoder.length; i++) {
                                    encoder.push(supportedEncoder[i])
                                }
                                return encoder
                            }
                        }

                        Label {
                            text: qsTr("Audio device")
                        }

                        AudioDeviceComboBox {

                            id: audioInput
                            Component.onCompleted: {
                                const deviceId = settings.value(
                                                   monitorDialog.monitorId + "/audioInputDevice",
                                                   audioInput.defaultInputDevice.id)
                                if (deviceId.length > 0) {
                                    audioInput.currentIndex = Math.max(
                                                audioInput.indexOfValue(
                                                    deviceId), 0)
                                } else {
                                    audioInput.currentIndex = 0
                                }
                            }
                        }

                        Label {
                            text: qsTr("Push to talk")
                        }

                        Row {
                            Switch {
                                id: pushToTalkSwitch
                                checked: settings.value(
                                             monitorDialog.monitorId + "/pushToTalk",
                                             true)
                            }
                        }

                        Label {
                            text: qsTr("Sensitivity")
                        }

                        Row {
                            Dial {
                                id: sensitivityDial
                                value: settings.value(
                                           monitorDialog.monitorId + "/micSensitivity",
                                           1.0)
                            }
                        }

                        Rally.Button {

                            id: talkButton
                            text: qsTr("Test")
                            icon.name: "microphone"
                            checkable: !pushToTalkSwitch.checked
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

            property var profileId: null

            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isPreviousItem
                asynchronous: true
                sourceComponent: firstPage
                visible: status == Loader.Ready
            }

            Loader {
                active: SwipeView.isCurrentItem
                asynchronous: true
                sourceComponent: secondPage
                visible: status == Loader.Ready
            }
        }
    }

    QtObject {
        id: priv
        property var mediaProfile: null
        property var deviceInfo: DeviceManager.getDeviceInfo(
                                     mediaProfile ? mediaProfile.profileId.getDeviceId(
                                                        ) : "")

        function saveSettings() {}
    }
}
