import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import Onvif
import MaterialRally as Rally

Pane {

    id: control

    property var mediaProfile: null
    property MonitorSettings settings: MonitorSettings {}

    ColumnLayout {

        id: columnLayoutPage2
        width: Math.min(parent.width, 600)
        anchors.horizontalCenter: parent.horizontalCenter

        CameraStream {

            profileId: control.mediaProfile.profileId
            Layout.fillWidth: true
            implicitHeight: width * videoHeight / videoWidth
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
                    Dial {
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

            Layout.fillWidth: true
            title: qsTr("Audio backchannel")
            icon.name: "bullhorn"
            enabled: control.mediaProfile.hasBackchannel
            mainAction: Rally.BusyAction {

                id: backchannelStreamSwitch
                checkable: true
                checked: settings.enableBackchannel
                onToggled: {
                    settings.enableBackchannel = checked
                    if (checked) {
                        rtpSource.start(control.mediaProfile.backchannelUrl)
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
                        const codecId = settings.audioCodec
                        if (codecId.length > 0) {
                            audioInput.currentIndex = Math.max(audioInput.find(
                                                                   codecId), 0)
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
                                                   control.mediaProfile.mediaDescription)
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
                            audioInput.currentIndex = Math.max(
                                        audioInput.indexOfValue(deviceId), 0)
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
                    Dial {
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
        }
    }
}
