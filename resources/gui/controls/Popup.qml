import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Window 2.12

T.Dialog {

    id: control

    readonly property real yStart: 80

    Material.elevation: 0
    topPadding: yStart

    parent: T.Overlay.overlay
    width: parent.width

    padding: 0

    function openWithAnimOffset(yOffset) {

        priv.yAnimOffset = yOffset
        control.open()
    }

    header: T.ToolBar {

        y: control.yStart

        Material.elevation: 0

        background: Item {}

        T.RoundButton {
            icon.name: "ic_arrow_back"
            flat: true
            onClicked: {
                control.close()
            }
        }
    }

    enter: Transition {

        SequentialAnimation {

            PauseAnimation {
                duration: 100
            }

            ParallelAnimation {
                NumberAnimation {
                    target: control.contentItem
                    property: "opacity"
                    duration: 400
                    easing.type: Easing.OutQuart
                    from: 0
                    to: 1
                }
                NumberAnimation {
                    target: control.header
                    property: "opacity"
                    duration: 400
                    easing.type: Easing.OutQuart
                    from: 0
                    to: 1
                }
            }
        }

        SequentialAnimation {

            PauseAnimation {
                duration: 100
            }

            ParallelAnimation {
                NumberAnimation {
                    targets: [control]
                    property: "topPadding"
                    duration: 400
                    from: control.yStart
                    to: 0
                    easing.type: Easing.OutQuart
                }
                NumberAnimation {
                    targets: [control.header]
                    property: "y"
                    duration: 400
                    from: control.yStart
                    to: 0
                    easing.type: Easing.OutQuart
                }
            }
        }
        NumberAnimation {
            property: "height"
            duration: 300
            from: 0.0
            to: control.parent.height
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "y"
            duration: 300
            from: priv.yAnimOffset
            to: 0
            easing.type: Easing.OutCubic
        }
        ColorAnimation {
            target: control.background
            property: "color"
            duration: 230
            from: control.Material.primaryColor
            to: Qt.lighter(control.Material.backgroundColor, 1.2)
            easing.type: Easing.OutCubic
        }
    }

    exit: Transition {
        SequentialAnimation {

            PauseAnimation {
                duration: 100
            }

            ParallelAnimation {
                NumberAnimation {
                    property: "height"
                    duration: 300
                    from: control.parent.height
                    to: 0.0
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    property: "y"
                    duration: 300
                    from: 0
                    to: priv.yAnimOffset
                    easing.type: Easing.OutCubic
                }
                ColorAnimation {
                    target: control.background
                    property: "color"
                    duration: 230
                    from: Qt.lighter(control.Material.backgroundColor, 1.2)
                    to: control.Material.backgroundColor
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    property: "opacity"
                    duration: 300
                    from: 1
                    to: 0
                    easing.type: Easing.OutCubic
                }
            }
        }

        NumberAnimation {
            target: control.contentItem
            property: "opacity"
            duration: 400
            easing.type: Easing.OutQuart
            from: 1
            to: 0
        }
        NumberAnimation {
            target: control.header
            property: "opacity"
            duration: 400
            easing.type: Easing.OutQuart
            from: 1
            to: 0
        }
        NumberAnimation {
            targets: [control]
            property: "topPadding"
            duration: 400
            from: 0
            to: control.yStart
            easing.type: Easing.OutQuart
        }
        NumberAnimation {
            targets: [control.header]
            property: "y"
            duration: 400
            from: 0
            to: control.yStart
            easing.type: Easing.OutQuart
        }
    }

    ScaleAnimator {
        id: animIn
        from: 1
        to: 0.85
        duration: 200
        running: false
        easing.type: Easing.InQuad
    }

    ScaleAnimator {
        id: an1mOut
        from: 0.85
        to: 1
        duration: 250
        running: false
        easing.type: Easing.OutQuad
    }

    onAboutToShow: {

        control.header.opacity = 0
        control.contentItem.opacity = 0
        control.header.y = control.yStart
        control.topPadding = control.yStart

        animIn.target = T.ApplicationWindow.window.contentItem
        animIn.start()
    }

    onAboutToHide: {

        animIn.stop()
        an1mOut.target = T.ApplicationWindow.window.contentItem
        an1mOut.start()
        //T.ApplicationWindow.window.contentItem.scale = 1
    }

    onOpened: {
        control.height = control.parent.height
        control.height = Qt.binding(function () {
            return control.parent.height
        })
    }

    onClosed: {

        control.opacity = 1

        // Make sure the main Window is at full scale again
        T.ApplicationWindow.window.contentItem.scale = 1
    }

    QtObject {
        id: priv
        property real yAnimOffset: control.parent.height / 2
    }
}