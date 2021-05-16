import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Item {

    id: conrol

    property var profileId
    property alias fillMode: image.fillMode
    property bool finished: image.status === Image.Ready ? true : false

    function refresh() {

        priv.load(profileId)
    }

    height: image.status === Image.Ready ? image.implicitHeight / image.implicitWidth
                                           * width : image.sourceSize.height
                                           / image.sourceSize.width * width

    onProfileIdChanged: {

        priv.load(profileId)
    }

    Image {

        id: image

        anchors.fill: parent

        mipmap: true
        asynchronous: false
        cache: false
        sourceSize.width: 1280
        sourceSize.height: 720

        Rectangle {

            anchors.fill: parent

            color: palette.base

            visible: image.status === Image.Ready ? false : true

            Icon {

                id: loadingIcon

                name: "ic_linked_camera"

                width: Math.round(Math.min(parent.width, parent.height) / 3.0)
                height: width

                visible: image.status === Image.Loading ? true : false

                anchors.centerIn: parent

                SequentialAnimation {

                    running: true
                    loops: Animation.Infinite
                    OpacityAnimator {

                        target: loadingIcon
                        from: 0
                        to: 1
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }

                    OpacityAnimator {

                        target: loadingIcon
                        from: 1
                        to: 0
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                }
            }

            Icon {

                id: loadingFailedIcon

                name: "ic_warning"

                width: Math.round(Math.min(parent.width, parent.height) / 3.0)
                height: width

                visible: image.status === Image.Error ? true : false

                anchors.centerIn: parent
            }
        }

        WRoundButton {

            anchors.right: parent.right
            anchors.margins: 10
            anchors.bottom: parent.bottom

            visible: image.status === Image.Loading ? false : true
            icon.name: "ic_refresh"

            onClicked: {

                priv.load(priv.profileId)
            }
        }
    }

    QtObject {

        id: priv
        property var profileId

        function load(profileId, timestamp) {

            console.debug("Camera snapshot requested for profile: " + profileId)
            priv.profileId = profileId
            if (!timestamp) {
                timestamp = new Date()
            }
            if (!profileId) {
                profileId = new ProfileId()
                console.warn("No profile id given")
            }

            image.source = "image://profile/" + profileId.getDeviceId(
                        ) + "/" + profileId.getProfileToken() + "/" + timestamp
        }
    }
}
