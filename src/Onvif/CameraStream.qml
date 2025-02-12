import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtMultimedia
import Onvif

Control {

    id: control

    property var profileId
    property MonitorSettings settings: MonitorSettings {}
    // Use FillMode enum
    property int fillMode: FillMode.PreserveAspectFit

    property alias state: player.state
    property alias videoWidth: player.videoWidth
    property alias videoHeight: player.videoHeight
    property alias disableVideo: player.disableVideo
    property alias disableAudio: player.disableAudio

    readonly property bool landscape: videoHeight / Math.max(videoWidth, 1) <= 1
    readonly property bool portrait: !landscape

    signal firstFrame

    clip: true

    implicitHeight: {

        if (control.videoHeight > 0) {
            return control.videoHeight
        }
        return 0
    }

    implicitWidth: {

        if (control.videoWidth > 0) {
            return control.videoWidth
        }
        return 0
    }

    Component.onCompleted: {

        player.firstFrame.connect(() => {
                                      loader.visible = false
                                      control.firstFrame()
                                  })
    }

    onProfileIdChanged: {

        loader.visible = true
        if (profileId) {
            player.source = DeviceManager.getStreamUrl(profileId.getDeviceId(), profileId.getProfileToken())
        } else {
            player.source = ""
        }
    }

    contentItem: MediaPlayer {

        id: player
        source: ""

        volume: settings.volume

        transform: [
            Scale {
                origin.x: width / 2
                origin.y: height / 2
                xScale: settings.mirrorHorizontal ? -1 : 1
                yScale: settings.mirrorVertical ? -1 : 1
            },
            Rotation {
                origin.x: width / 2
                origin.y: height / 2
                angle: settings.rotation
            },
            Scale {
                origin.x: width / 2
                origin.y: height / 2
                xScale: settings.zoom
                yScale: settings.zoom
            }
        ]

        Component.onCompleted: {
            player.play() // to early, will stopped by setSource()
        }
    }

    background: Rectangle {

        color: "black"
    }

    CameraLoadingIndicator {

        id: loader
        anchors.fill: control
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
                        rtpSource.start(DeviceManager.getStreamUrl(profileId.getDeviceId(),
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
}
