import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import Onvif
import MaterialRally as Rally

Control {

    id: conrol

    property var profileId
    property MonitorSettings settings: MonitorSettings {}
    // Use FillMode enum
    property int fillMode: FillMode.PreserveAspectFit

    property bool finished: true

    property int imageWidth: image.implicitWidth
    property int imageHeight: image.implicitHeight
    property bool autoReload: false
    property alias autoReloadInterval: autoReloadTimer.interval

    readonly property bool landscape: imageHeight / Math.max(imageWidth, 1) <= 1
    readonly property bool portrait: !landscape

    clip: true

    implicitHeight: imageHeight
    implicitWidth: imageWidth

    function refresh() {

        priv.load(profileId)
    }

    function clear() {

        image.source = ""
    }


    /*
    function clear() {

        for (var i = 0; i < imageContainer.children.length; i++) {
            const child = imageContainer.children[i]
            child.visible = false
            child.destroy()
        }
    }
    */
    onVisibleChanged: {

        if (!conrol.visible)
            clear()
        else
            refresh()
    }

    onProfileIdChanged: {

        loader.visible = true
        if (profileId) {
            priv.load(profileId)
        } else {
            image.source = ""
        }
    }

    contentItem: Image {

        id: image

        mipmap: true
        cache: false
        asynchronous: false
        retainWhileLoading: true
        sourceSize.width: 720
        sourceSize.height: 1280
        fillMode: {
            if (conrol.fillMode === FillMode.PreserveAspectFit) {
                return Image.PreserveAspectFit
            } else if (conrol.fillMode === FillMode.PreserveAspectCrop) {
                return Image.PreserveAspectCrop
            }
            return Image.Stretch
        }

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

        onStatusChanged: {
            if (image.status === Image.Ready) {
                loader.isError = false
                loader.visible = false
            } else if (image.status === Image.Error) {
                loader.isError = true
                loader.visible = true
            }
        }
    }

    background: Rectangle {

        color: "black"
    }

    CameraLoadingIndicator {

        id: loader
        anchors.fill: image
    }

    Timer {
        id: autoReloadTimer
        running: conrol.autoReload && conrol.visible && conrol.profileId != null
        interval: 10000
        repeat: true
        onTriggered: {

            conrol.refresh()
        }
    }


    /*
    Component {

        id: imageComponent

        Item {

            property alias status: image.status
            property alias source: image.source
            implicitWidth: image.implicitWidth
            implicitHeight: image.implicitHeight

            Image {

                id: image

                anchors.fill: parent

                mipmap: true
                cache: false
                asynchronous: false
                sourceSize.width: 720
                sourceSize.height: 1280
                fillMode: {
                    if (conrol.fillMode === FillMode.PreserveAspectFit) {
                        return Image.PreserveAspectFit
                    } else if (conrol.fillMode === FillMode.PreserveAspectCrop) {
                        return Image.PreserveAspectCrop
                    }
                    return Image.Stretch
                }

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
            }



            WRoundButton {

                anchors.right: parent.right
                anchors.margins: 10
                anchors.bottom: parent.bottom

                visible: false // image.status === Image.Loading ? false : true
                icon.name: "refresh"

                onClicked: {

                    image.source = priv.createImageUrl(priv.profileId)
                }
            }
        }
    }
*/
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

            return "image://profile/" + profileId.getDeviceId() + "/" + profileId.getProfileToken() + "/" + timestamp
        }

        function load(profileId) {

            priv.profileId = profileId

            const imageSource = priv.createImageUrl(profileId)

            image.source = imageSource
        }


        /*
        function load(profileId) {

            priv.profileId = profileId

            const imageSource = priv.createImageUrl(profileId)

            let image = imageComponent.createObject(imageContainer, {
                                                        "anchors.fill": imageContainer,
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
        }*/
    }
}
