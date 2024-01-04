import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtMultimedia
import Onvif

Control {

    property var profileId
    property MonitorSettings settings: MonitorSettings {}

    property alias videoWidth: player.videoWidth
    property alias videoHeight: player.videoHeight
    property alias disableVideo: player.disableVideo
    property alias disableAudio: player.disableAudio

    implicitWidth: videoWidth
    implicitHeight: videoHeight

    onProfileIdChanged: {

        if (profileId) {
            player.source = DeviceManager.getStreamUrl(
                        profileId.getDeviceId(), profileId.getProfileToken())
        } else {
            player.source = ""
        }
    }

    MediaPlayer {
        id: player
        source: ""
        anchors.fill: parent
        Component.onCompleted: player.play(
                                   ) // to early, will stopped by setSource()
    }

    Loader {

        active: settings.enableBackchannel

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: parent.width * 0.03
        anchors.rightMargin: parent.width * 0.03

        sourceComponent: Component {

            id: backchannelButtonComponent

            RoundButton {

                id: talkButton

                MediaDevices {
                    id: devices
                }

                MicrophoneRtpSource {

                    id: rtpSource

                    Component.onCompleted: {
                        rtpSource.start(DeviceManager.getStreamUrl(
                                            profileId.getDeviceId(),
                                            profileId.getProfileToken()))
                    }

                    payloadFormat: MicrophoneRtpSource.RTP_PCMU_8000_1
                    audioInput: AudioInput {
                        device: {
                            for (var i = 0; i < devices.audioInputs.length; i++) {
                                if (devices.audioInputs[i].mode === AudioDevice.Input) {
                                    if (devices.audioInputs[i].id === settings.audioInputDevice) {
                                        return devices.audioInputs[i]
                                    }
                                }
                            }
                            return devices.defaultAudioInput
                        }
                        volume: settings.micSensitivity
                        muted: talkButton.checkable ? !talkButton.checked : !talkButton.down
                    }
                }

                checkable: !settings.pushToTalk
                icon.name: "microphone"
                width: 66
                height: 66
                icon.width: width / 2
                icon.height: height / 2
                Material.elevation: 3
                Material.background: Material.accent
            }
        }
    }

    background: Rectangle {

        color: "black"
    }
}
