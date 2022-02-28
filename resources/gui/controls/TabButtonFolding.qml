import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

T.Button {

    id: control

    flat: true
    checkable: true
    font.capitalization: Font.AllUppercase
    font.weight: Font.Black
    font.letterSpacing: 1.4
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

    Behavior on implicitWidth {
        SmoothedAnimation {
            duration: 200
            velocity: -1
        }
    }
}