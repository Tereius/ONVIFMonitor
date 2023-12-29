import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts
import QuickFuture
import Onvif
import MaterialRally as Rally

Rally.Popup {

    id: credentialsDialog

    title: credentialsDialog.deviceId ? qsTr("Edit Device") : qsTr(
                                            "Add new Device")

    onBackButtonClicked: {

        credentialsDialog.close()
    }

    property var deviceId: null
    readonly property string deviceName: ""
    readonly property string deviceEndpoint: ""
    readonly property bool deviceNameFixed: false
    readonly property bool deviceEndpointFixed: false
    readonly property bool isNew: !DeviceManager.containsDevice(
                                      credentialsDialog.deviceId)

    actions: [
        Rally.BusyAction {
            id: deleteAction
            text: qsTr("Delete")
            icon.name: "delete"
            enabled: !credentialsDialog.isNew
            onTriggered: {
                priv.deleteDevice()
            }
        },

        Rally.BusyAction {
            id: addAction
            text: credentialsDialog.isNew ? qsTr("Add") : qsTr("Save")
            icon.name: "check"
            enabled: deviceNameField.acceptableInput
                     && deviceNameField.text.length > 0
                     && hostField.acceptableInput && hostField.text.length > 0
            onTriggered: {
                priv.editDevice()
            }
        }
    ]

    onDeviceIdChanged: {
        priv.initDialog()
    }

    function reload() {
        credentialsDialog.deviceId = credentialsDialog.deviceId
    }

    Rally.ScrollView {

        id: scrollView
        anchors.fill: parent
        Keys.onEnterPressed: priv.editDevice()
        Keys.onReturnPressed: priv.editDevice()
        enableVerticalScrollBar: credentialsDialog.opened
        contentHeight: columnLayout.implicitHeight

        ColumnLayout {

            id: columnLayout
            implicitWidth: Math.min(scrollView.availableWidth, 600)
            anchors.horizontalCenter: parent.horizontalCenter

            Item {

                Layout.fillWidth: true
                implicitHeight: message.count === 0 ? 0 : message.implicitHeight

                Behavior on implicitHeight {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.OutCubic
                    }
                }

                Rally.InlineMessage {
                    id: message
                    title: qsTr("Error")
                    width: parent.width

                    Behavior on implicitHeight {
                        enabled: false
                    }
                }
            }

            Rally.GroupBox {

                title: qsTr("Basic settings")
                icon.name: "cog"
                Layout.fillWidth: true

                Rally.FormLayout {

                    width: parent.width

                    Label {
                        text: qsTr("Device name")
                    }

                    Rally.TextField {

                        id: deviceNameField
                        text: credentialsDialog.deviceName
                        focus: true
                        enabled: !credentialsDialog.deviceNameFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    Label {
                        text: qsTr("Device endpoint")
                    }

                    Rally.TextField {

                        id: hostField
                        text: credentialsDialog.deviceEndpoint
                        enabled: !credentialsDialog.deviceEndpointFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    Label {
                        text: qsTr("Credentials")
                    }

                    Rally.TextField {

                        id: userField
                        placeholderText: qsTr("User")
                        font.family: "Roboto Mono"
                    }

                    Rally.PasswordTextField {

                        id: passwordField
                        placeholderText: qsTr("Password")
                        font.family: "Roboto Mono"
                    }
                }
            }

            Loader {
                sourceComponent: DeviceManager.isDeviceInitialized(
                                     credentialsDialog.deviceId) ? d : undefined
                Layout.fillWidth: true
            }

            Component {

                id: d

                Rally.GroupBox {

                    title: qsTr("Audio Backchannel")
                    icon.name: "bullhorn"

                    Rally.FormLayout {

                        width: parent.width

                        Label {
                            text: qsTr("This device supports audio backchannel")
                        }

                        Rally.ComboBox {

                            id: mediaProfile
                            model: {
                                return DeviceManager.getDeviceInfo(
                                            credentialsDialog.deviceId).mediaProfiles
                            }
                            currentIndex: 0
                            Layout.fillWidth: true
                            placeholderText: qsTr("Stream profile")
                            textRole: "name"
                        }

                        CameraStream {

                            profileId: mediaProfile.currentValue.profileId
                            width: 200
                            height: 200
                        }

                        Rally.ComboBox {

                            id: codecs
                            model: {
                                return rtpSource.supportedEncoder(
                                            mediaProfile.currentValue.mediaDescription)
                            }
                            currentIndex: 0
                            Layout.fillWidth: true
                            placeholderText: qsTr("Codecs")
                            textRole: "codec"
                        }

                        AudioDeviceComboBox {
                            id: audioInput
                            Layout.fillWidth: true
                        }

                        Switch {
                            id: muteSwitch
                        }

                        Dial {
                            id: volumeDial
                            value: 1.0
                        }

                        Button {

                            MicrophoneRtpSource {

                                id: rtpSource
                                payloadFormat: MicrophoneRtpSource.RTP_PCMU_8000_1
                                audioInput: AudioInput {
                                    device: audioInput.currentValue
                                    volume: volumeDial.value
                                    muted: muteSwitch.checked
                                }
                            }

                            text: qsTr("Test")
                            icon.name: "microphone"
                            Layout.fillWidth: true
                            checkable: true
                            onToggled: {
                                if (checked) {
                                    rtpSource.start(
                                                mediaProfile.currentValue.backchannelUrl)
                                } else {
                                    rtpSource.stop()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    QtObject {
        id: priv

        function initDialog() {

            if (credentialsDialog.deviceId && DeviceManager.containsDevice(
                        credentialsDialog.deviceId)) {
                const daviceInfo = DeviceManager.getDeviceInfo(
                                     credentialsDialog.deviceId)
                deviceNameField.text = DeviceManager.getName(
                            credentialsDialog.deviceId)
                hostField.text = daviceInfo.deviceEndpoint
                userField.text = daviceInfo.user
                passwordField.text = daviceInfo.password

                if (!daviceInfo.initialized) {
                    message.pushMessage(daviceInfo.initializationError,
                                        "error", qsTr("Initialization error"))
                }
            }
        }

        function editDevice() {

            if (credentialsDialog.isNew) {

                // Add device case
                credentialsDialog.busy = true
                let future = DeviceManager.addDevice(
                        hostField.text, userField.text, passwordField.text,
                        deviceNameField.text,
                        credentialsDialog.deviceId ? credentialsDialog.deviceId : Onvif.createUuid(
                                                         ))
                Future.onFinished(future, function (result) {

                    if (!result.isSuccess()) {
                        message.pushMessage(result.getDetails(), "error",
                                            qsTr("Initialization error"))
                    } else {
                        credentialsDialog.reload()
                    }
                    credentialsDialog.busy = false
                }, function () {
                    credentialsDialog.busy = false
                })
            } else {

                // Edit device case
                credentialsDialog.busy = true
                DeviceManager.renameDevice(credentialsDialog.deviceId,
                                           deviceNameField.text)

                let future = DeviceManager.setDeviceCredentials(
                        credentialsDialog.deviceId, userField.text,
                        passwordField.text)

                Future.onFinished(future, function (result) {

                    if (!result.isSuccess()) {
                        message.pushMessage(result.getDetails(), "error",
                                            qsTr("Initialization error"))
                    }
                    credentialsDialog.reload()
                    credentialsDialog.busy = false
                }, function () {
                    credentialsDialog.busy = false
                })
            }
        }

        function deleteDevice() {

            if (credentialsDialog.deviceId) {
                DeviceManager.removeDevice(credentialsDialog.deviceId)
                credentialsDialog.close()
            }
        }
    }
}
