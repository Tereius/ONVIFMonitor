import QtCore
import QtQuick
import QtQuick.Controls.Material
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import QuickFuture
import Onvif
import MaterialRally as Rally

Pane {

    id: control

    property var mediaProfile: null
    property MonitorSettings settings: MonitorSettings {}

    ColumnLayout {

        id: layout
        width: Math.min(control.width, 600)
        anchors.horizontalCenter: parent.horizontalCenter

        CameraStream {
            id: cameraStream
            profileId: control.mediaProfile.profileId
            settings: control.settings
            Layout.fillWidth: true
            Layout.preferredHeight: cameraStream.videoHeight
                                    && cameraStream.videoWidth ? (cameraStream.width * cameraStream.videoHeight
                                                                  / cameraStream.videoWidth) : (cameraStream.width * 720 / 1280)
        }

        Rally.GroupBox {

            Layout.fillWidth: true
            title: qsTr("Video stream")
            icon.name: "cctv"
            mainAction: Rally.BusyAction {
                checkable: true
                checked: settings.enableVideoStream
                onToggled: {
                    settings.enableVideoStream = checked
                }
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
                        checked: settings.mirrorHorizontal
                        onToggled: {
                            settings.mirrorHorizontal = checked
                        }
                    }

                    Button {
                        flat: true
                        icon.name: "flip-vertical"
                        checkable: true
                        display: AbstractButton.IconOnly
                        scale: checked ? -1 : 1
                        checked: settings.mirrorVertical
                        onToggled: {
                            settings.mirrorVertical = checked
                        }
                    }
                }

                Label {
                    text: qsTr("Rotation/Zoom")
                }

                RowLayout {

                    SpinBox {
                        from: -360
                        to: 360
                        editable: true
                        value: settings.rotation
                        onValueModified: {
                            settings.rotation = value
                        }

                        textFromValue: function (value, locale) {
                            return value + "°"
                        }

                        valueFromText: function (text, locale) {
                            return text.replace("°", "")
                        }
                    }

                    SpinBox {
                        from: 100
                        to: 500
                        editable: true
                        value: settings.zoom * 100.0
                        onValueModified: {
                            settings.zoom = value / 100.0
                        }

                        textFromValue: function (value, locale) {
                            return value + "%"
                        }

                        valueFromText: function (text, locale) {
                            return text.replace("%", "")
                        }
                    }
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
                checked: settings.enableAudioStream
                onToggled: {
                    settings.enableAudioStream = checked
                }
            }

            Rally.FormLayout {

                width: parent.width

                Label {
                    text: qsTr("Volume")
                }

                Row {
                    VolumeDial {
                        id: volume
                        value: settings.volume
                        onMoved: {
                            settings.volume = value
                        }
                    }
                }
            }
        }

        Rally.GroupBox {

            id: backchannelGroup
            property bool initializing: false
            property bool hasError: false
            property string errorMessage: ""

            Layout.fillWidth: true
            title: qsTr("Audio backchannel")
            icon.name: "microphone"
            enabled: control.mediaProfile.hasBackchannel
            mainAction: Rally.BusyAction {

                id: backchannelStreamSwitch
                checkable: true
                checked: settings.enableBackchannel
                busy: backchannelGroup.initializing
                onToggled: {
                    settings.enableBackchannel = checked
                    if (checked) {
                        backchannelGroup.initializing = true

                        let future = rtpSource.start(control.mediaProfile.backchannelUrl)

                        Future.onFinished(future, function (result) {
                            if (result.isFault()) {
                                backchannelGroup.errorMessage = result.getDetails()
                                backchannelGroup.hasError = true
                            } else {
                                backchannelGroup.errorMessage = ""
                                backchannelGroup.hasError = false
                            }
                            backchannelGroup.initializing = false
                        }, function () {
                            backchannelGroup.errorMessage = ""
                            backchannelGroup.initializing = false
                            backchannelGroup.hasError = false
                        })
                    } else {
                        rtpSource.stop()
                    }
                }
            }

            contentHeight: backchannelGroup.initializing ? 0 : backchannelForm.implicitHeight

            MicrophoneRtpSource {

                id: rtpSource
                payloadFormat: MicrophoneRtpSource.RTP_PCMU_8000_1
                audioInput: AudioInput {
                    device: audioInput.currentValue
                    volume: sensitivityDial.value
                    muted: talkButton.checkable ? !talkButton.checked : !talkButton.down
                }
            }

            ColumnLayout {

                id: backchannelForm
                width: parent.width

                Rally.FormLayout {

                    Layout.fillWidth: true
                    visible: !backchannelGroup.initializing && !backchannelGroup.hasError

                    Label {
                        text: qsTr("Codec")
                    }

                    Rally.ComboBox {

                        id: codecs
                        currentIndex: 0
                        textRole: "codec"
                        Component.onCompleted: {
                            const codecId = settings.audioCodec
                            if (codecId.length > 0) {
                                audioInput.currentIndex = Math.max(audioInput.find(codecId), 0)
                            } else {
                                audioInput.currentIndex = 0
                            }
                        }
                        model: {
                            let encoder = [{
                                               "id": "Auto",
                                               "codec": "Auto"
                                           }]
                            const supportedEncoder = rtpSource.supportedEncoder(control.mediaProfile.mediaDescription)
                            for (var i = 0; i < supportedEncoder.length; i++) {
                                encoder.push(supportedEncoder[i])
                            }
                            return encoder
                        }
                        onActivated: {
                            settings.audioCodec = currentText
                        }
                    }

                    Label {
                        text: qsTr("Audio device")
                    }

                    AudioDeviceComboBox {

                        id: audioInput
                        Component.onCompleted: {
                            const deviceId = settings.audioInputDevice
                            if (deviceId.length > 0) {
                                audioInput.currentIndex = Math.max(audioInput.indexOfValue(deviceId), 0)
                            } else {
                                audioInput.currentIndex = 0
                            }
                        }
                        onActivated: {
                            settings.audioInputDevice = currentValue.id
                        }
                    }

                    Label {
                        text: qsTr("Push to talk")
                    }

                    Row {
                        Switch {
                            id: pushToTalkSwitch
                            checked: settings.pushToTalk
                            onToggled: {
                                settings.pushToTalk = checked
                            }
                        }
                    }

                    Label {
                        text: qsTr("Sensitivity")
                    }

                    Row {
                        VolumeDial {
                            id: sensitivityDial
                            value: settings.micSensitivity
                            onMoved: {
                                settings.micSensitivity = value
                            }
                        }
                    }

                    Rally.Button {

                        id: talkButton
                        text: qsTr("Test")
                        icon.name: "microphone"
                        checkable: !pushToTalkSwitch.checked
                    }
                }

                Placeholder {

                    Layout.fillWidth: true
                    visible: !backchannelGroup.initializing && backchannelGroup.hasError
                    text: qsTr("Failed to initialize audio backchannel: %1").arg(backchannelGroup.errorMessage)
                    icon.name: "alert-outline"
                    icon.color: Material.color(Material.Red)
                }
            }
        }
    }
}
