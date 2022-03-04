import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

T.TextField {

    id: control
    selectByMouse: true

    placeholderTextColor: control.acceptableInput ? Material.secondaryTextColor : Material.color(
                                                        Material.Red)
    topPadding: control.placeholderText.length > 0 ? 22 : 10
    bottomPadding: control.placeholderText.length > 0 ? 10 : 10
    leftPadding: 12
    rightPadding: 12
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    autoScroll: false // TODO: Workaround - show the beginning of the text if text is longer than the text field

    onActiveFocusChanged: {
        control.autoScroll = true
    }

    Component.onCompleted: {
        // Delete the default PlaceholderText
        for (var key in children) {
            const child = children[key]
            if (child && child !== control.background
                    && child !== placeholder) {
                child.visible = false
            }
        }
    }

    PlaceholderText {
        id: placeholder

        property bool raise: control.activeFocus || control.length > 0

        x: control.leftPadding
        y: placeholder.raise ? 4 : (control.topPadding + control.bottomPadding) / 2

        Behavior on y {
            SmoothedAnimation {
                duration: 250
                velocity: -1
            }
        }

        Behavior on scale {
            SmoothedAnimation {
                duration: 250
                velocity: -1
            }
        }

        scale: placeholder.raise ? 0.75 : 1
        transformOrigin: Item.TopLeft
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)
        text: control.placeholderText + (control.acceptableInput ? "" : "*")
        font: control.font
        color: control.placeholderTextColor
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        renderType: control.renderType
        visible: !control.activeFocus
                 || control.horizontalAlignment !== Qt.AlignHCenter
    }

    background: Rectangle {
        id: backgr
        implicitWidth: 250
        implicitHeight: control.Material.buttonHeight

        color: !control.enabled ? control.Material.buttonDisabledColor : control.highlighted ? control.Material.highlightedRippleColor : control.Material.rippleColor

        // Trigger the hover effect manually
        property bool manualHover: false

        Rectangle {

            width: parent.width
            height: control.activeFocus || control.hovered
                    || backgr.manualHover ? 2 : 1
            anchors.bottom: parent.bottom
            antialiasing: true

            Behavior on height {
                SmoothedAnimation {
                    duration: 250
                    velocity: -1
                }
            }

            color: {
                if (control.activeFocus) {
                    if (control.acceptableInput) {
                        control.Material.accentColor
                    } else {
                        Material.color(Material.Red)
                    }
                } else {
                    if (control.hovered || backgr.manualHover) {
                        if (control.acceptableInput) {
                            control.Material.primaryTextColor
                        } else {
                            Material.color(Material.Red)
                        }
                    } else {
                        if (control.acceptableInput) {
                            control.Material.hintTextColor
                        } else {
                            Material.color(Material.Red)
                        }
                    }
                }
            }
        }
    }
}
