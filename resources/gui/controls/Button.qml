import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

T.Button {

    id: control

    font.capitalization: Font.AllUppercase
    font.letterSpacing: 1.6
    font.pixelSize: 12

    padding: 18
    horizontalPadding: padding
    spacing: 6

    icon.width: 17
    icon.height: 17

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: control.Material.buttonHeight

        radius: 4
        color: !control.enabled ? control.Material.buttonDisabledColor : control.highlighted ? control.Material.highlightedButtonColor : control.Material.buttonColor

        Ripple {
            clipRadius: 2
            width: parent.width
            height: parent.height
            pressed: control.pressed
            anchor: control
            active: control.down || control.visualFocus || control.hovered
            color: control.flat
                   && control.highlighted ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }
}
