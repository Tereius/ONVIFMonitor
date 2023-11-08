import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import MaterialRally as Rally

Item {

    id: conrol

    property var profileId
    //property var fillMode: image.fillMode
    property bool finished: true //image.status === Image.Ready ? true : false

    property bool autoReload: false
    property alias autoReloadInterval: autoReloadTimer.interval

    property int imageHeight: {
        let children = imageContainer.children
        if (children.length) {
            return children[0].implicitHeight
        }
    }

    property int imageWidth: {
        let children = imageContainer.children
        if (children.length) {
            return children[0].implicitWidth
        }
    }

    implicitHeight: imageHeight

    implicitWidth: imageWidth

    function refresh() {

        priv.load(profileId)
    }

    function clear() {

        for (var i = 0; i < imageContainer.children.length; i++) {
            const child = imageContainer.children[i]
            child.visible = false
            child.destroy()
        }
    }

    //height: image.status === Image.Ready ? image.implicitHeight / image.implicitWidth
    //                                     * width : image.sourceSize.height
    //                                   / image.sourceSize.width * width
    onVisibleChanged: {

        if (!conrol.visible)
            clear()
        else
            refresh()
    }

    onProfileIdChanged: {

        console.info("ProfileId changed")
        priv.load(profileId)
    }

    Timer {
        id: autoReloadTimer
        running: conrol.autoReload && conrol.visible && conrol.profileId != null
        interval: 10000
        repeat: true
        onTriggered: {
            //if (image.status !== Image.Loading) {
            conrol.refresh()
            //}
        }
    }

    Item {
        id: imageContainer

        anchors.fill: parent
    }

    Component {

        id: imageComponent

        Image {

            id: image

            anchors.fill: parent

            mipmap: true
            asynchronous: false
            cache: false
            sourceSize.width: 720
            sourceSize.height: 1280

            Rectangle {

                anchors.fill: parent

                color: palette.base

                visible: image.status === Image.Ready ? false : true

                Icon {

                    id: loadingIcon

                    name: "camera-wireless"

                    width: Math.round(Math.min(parent.width,
                                               parent.height) / 3.0)
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

                    name: "alert"

                    width: Math.round(Math.min(parent.width,
                                               parent.height) / 3.0)
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
                icon.name: "refresh"

                onClicked: {

                    image.source = priv.createImageUrl(priv.profileId)
                }
            }
        }
    }

    QtObject {

        id: priv
        property var profileId

        function createImageUrl(profileId, timestamp) {

            if (!timestamp) {
                timestamp = new Date()
            }
            if (!profileId) {
                profileId = new ProfileId()
                console.warn("No profile id given")
            }

            return "image://profile/" + profileId.getDeviceId(
                        ) + "/" + profileId.getProfileToken() + "/" + timestamp
        }

        function load(profileId) {

            console.debug("Camera snapshot requested for profile: " + profileId)
            priv.profileId = profileId

            const imageSource = priv.createImageUrl(profileId)

            let image = imageComponent.createObject(imageContainer, {
                                                        "source": imageSource,
                                                        "z": -1
                                                    })

            image.onStatusChanged.connect(function (status) {
                if (status === Image.Ready || status === Image.Error) {
                    image.z = 0
                    for (var i = 0; i < imageContainer.children.length; i++) {
                        const child = imageContainer.children[i]
                        if (child !== image) {
                            child.visible = false
                            child.destroy()
                        }
                    }
                }
            })
        }
    }
}
