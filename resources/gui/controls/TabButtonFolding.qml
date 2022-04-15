import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtGraphicalEffects 1.12

T.Button {

    id: control

    flat: true
    checkable: true
    font.capitalization: Font.AllUppercase
    font.styleName: "Bold"
    font.letterSpacing: 2.8
    icon.width: 22
    icon.height: 22
    implicitHeight: button.implicitHeight
    implicitWidth: Math.max(
                       button.implicitWidth + (button.checked ? label.implicitWidth : 0),
                       button.checked ? expandWidth : button.implicitWidth)

    property real expandWidth: 0
    property real proposedWidth: button.implicitWidth + label.implicitWidth

    background: Rectangle {
        color: "transparent"
        width: 0
        height: 0
    }

    contentItem: Rectangle {
        color: "transparent"
        width: 0
        height: 0
    }

    focusPolicy: Qt.NoFocus

    T.RoundButton {

        id: button

        flat: true
        opacity: control.checked ? 1 : 0.6
        checked: control.checked
        display: T.AbstractButton.IconOnly
        icon: control.icon
        checkable: !button.checked
        font: control.font

        onToggled: {
            control.toggle()
            control.toggled()
        }
    }

    DropShadow {
        anchors.fill: button
        radius: 4
        samples: radius * 2 + 1
        color: control.Material.dropShadowColor
        source: button
        visible: control.checked
    }

    T.Label {
        id: label
        text: control.text
        font: control.font
        anchors.verticalCenter: button.verticalCenter
        anchors.left: button.right

        state: button.checked ? "CHECKED" : "UNCHECKED"
        states: [
            State {
                name: "CHECKED"
                PropertyChanges {
                    target: label
                    opacity: 1
                }
            },
            State {
                name: "UNCHECKED"
                PropertyChanges {
                    target: label
                    opacity: 0
                }
            }
        ]

        transitions: [
            Transition {
                from: "CHECKED"
                to: "UNCHECKED"
                OpacityAnimator {
                    target: label
                    duration: 100
                }
            },
            Transition {
                from: "UNCHECKED"
                to: "CHECKED"
                SequentialAnimation {
                    PauseAnimation {
                        duration: 50
                    }
                    OpacityAnimator {
                        target: label
                        duration: 150
                        easing.type: Easing.InQuad
                    }
                }
            }
        ]
    }

    DropShadow {
        anchors.fill: label
        radius: 4
        samples: radius * 2 + 1
        color: control.Material.dropShadowColor
        source: label
        opacity: label.opacity
    }

    Behavior on implicitWidth {
        SmoothedAnimation {
            duration: 200
            velocity: -1
        }
    }
}
