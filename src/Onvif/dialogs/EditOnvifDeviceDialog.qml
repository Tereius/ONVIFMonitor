import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts
import QuickFuture
import Onvif
import MaterialRally as Controls

Controls.Popup {

    title: qsTr("Edit Device")

    id: credentialsDialog
    property string deviceName: ""
    property string deviceEndpoint: ""
    property var deviceId: null
    property bool deviceNameFixed: false
    property bool deviceEndpointFixed: false

    actions: [
        Controls.BusyAction {
            id: addAction
            text: qsTr("Add")
            icon.name: "check"
            icon.color: "red"
            enabled: deviceNameField.acceptableInput
                     && deviceNameField.text.length > 0
                     && hostField.acceptableInput && hostField.text.length > 0
            onTriggered: {
                priv.addDevice()
            }
        }
    ]

    Component.onCompleted: {
        const daviceInfo = DeviceManager.getDeviceInfo(
                             credentialsDialog.deviceId)

        deviceNameField.text = DeviceManager.getName(credentialsDialog.deviceId)
        hostField.text = daviceInfo.deviceEndpoint
        userField.text = daviceInfo.user
        passwordField.text = daviceInfo.password
    }

    ScrollView {

        anchors.fill: parent
        Keys.onEnterPressed: priv.addDevice()
        Keys.onReturnPressed: priv.addDevice()

        ColumnLayout {

            width: credentialsDialog.width

            Item {

                Layout.fillWidth: true
                implicitHeight: message.count === 0 ? 0 : message.implicitHeight

                Behavior on implicitHeight {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.OutCubic
                    }
                }

                Controls.InlineMessage {
                    id: message
                    title: qsTr("Error")
                    width: parent.width

                    Behavior on implicitHeight {
                        enabled: false
                    }
                }
            }

            Controls.GroupBox {

                Layout.fillWidth: true

                ColumnLayout {

                    spacing: 20
                    anchors.fill: parent

                    TextField {

                        id: deviceNameField

                        text: credentialsDialog.deviceName
                        focus: true
                        Layout.fillWidth: true
                        activeFocusOnTab: true
                        placeholderText: qsTr("Device Name")
                        selectByMouse: true
                        enabled: !credentialsDialog.deviceNameFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    TextField {

                        id: hostField

                        text: credentialsDialog.deviceEndpoint
                        Layout.fillWidth: true
                        activeFocusOnTab: true
                        placeholderText: qsTr("Device Host")
                        selectByMouse: true
                        enabled: !credentialsDialog.deviceEndpointFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    TextField {

                        id: userField

                        Layout.fillWidth: true
                        placeholderText: qsTr("User")
                        activeFocusOnTab: true
                        selectByMouse: true
                    }

                    TextField {

                        id: passwordField

                        echoMode: TextInput.Password
                        Layout.fillWidth: true
                        placeholderText: qsTr("Password")
                        activeFocusOnTab: true
                        selectByMouse: true
                    }
                }
            }

            Controls.GroupBox {
                title: qsTr("Audio Backchannel")
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent

                    ComboBox {
                        id: mediaProfile
                        model: mediaProfilesModel
                        currentIndex: 0
                        Layout.fillWidth: true
                        textRole: "name"
                        valueRole: "profileId"
                    }

                    ComboBox {
                        id: audioInput
                        model: devices.audioInputs
                        Layout.fillWidth: true
                        textRole: "description"
                    }

                    Switch {
                        id: muteSwitch
                    }

                    Dial {
                        id: volumeDial
                        value: 1.0
                    }

                    Button {
                        text: qsTr("Test")
                        icon.name: "microphone"
                        Layout.fillWidth: true
                        checkable: true
                        onToggled: {
                            if (checked) {
                                rtpSource.start(
                                            DeviceManager.getStreamUrl(
                                                credentialsDialog.deviceId,
                                                mediaProfile.currentValue.getProfileToken(
                                                    )))
                            } else {
                                rtpSource.stop()
                            }
                        }
                    }
                }
            }
        }
    }

    MediaDevices {
        id: devices
    }

    MicrophoneRtpSource {

        id: rtpSource
        payloadFormat: MicrophoneRtpSource.RTP_PCMU_8000_1
        audioInput: AudioInput {
            device: devices.audioInputs[audioInput.currentIndex]
            volume: volumeDial.value
            muted: muteSwitch.checked
        }
    }

    MediaProfilesModel {
        id: mediaProfilesModel
        deviceId: credentialsDialog.deviceId
    }

    QtObject {
        id: priv

        function addDevice() {
            credentialsDialog.busy = true

            DeviceManager.renameDevice(credentialsDialog.deviceId,
                                       deviceNameField.text)

            let future = DeviceManager.setDeviceCredentials(
                    credentialsDialog.deviceId, userField.text,
                    passwordField.text)

            Future.onFinished(future, function (result) {

                if (result.isSuccess()) {
                    credentialsDialog.close()
                } else {
                    message.pushMessage(result.getDetails(), "error", "Error")
                }
                credentialsDialog.busy = false
            }, function () {
                credentialsDialog.busy = false
            })
        }
    }
}
