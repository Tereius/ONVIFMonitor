import QtQuick
import QtQuick.Controls
import Onvif

Control {

    property var profileId

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

    background: Rectangle {

        color: "black"
    }
}
