import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import Onvif

Control {

    property var profileId

    onProfileIdChanged: {

        player.source = DeviceManager.getStreamUrl(profileId.getDeviceId(),
                                                   profileId.getProfileToken())
    }

    MediaPlayer {
        id: player
        source: ""
        anchors.fill: parent
        Component.onCompleted: player.play(
                                   ) // to early, will stopped by setSource()
    }
}
