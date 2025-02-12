import QtQuick
import MaterialRally as Rally

Rectangle {

    id: control

    property bool isError: false

    color: "black"

    Rally.Icon {

        id: loadingIcon

        name: "camera-wireless"

        width: Math.min(Math.round(Math.min(parent.width, parent.height) / 3.0), 40)
        height: width

        visible: !control.isError

        anchors.centerIn: control

        SequentialAnimation {

            running: loadingIcon.visible
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

    Rally.Icon {

        id: loadingFailedIcon

        name: "alert"

        width: Math.min(Math.round(Math.min(parent.width, parent.height) / 3.0), 40)
        height: width

        visible: control.isError

        anchors.centerIn: control
    }
}
