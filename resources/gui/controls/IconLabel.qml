import QtQuick 2.12
import QtQuick.Controls 2.12 as B
import QtQuick.Controls.impl 2.12 as T
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

B.AbstractButton {

    contentItem: T.IconLabel {

        id: control

        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: !control.enabled ? control.Material.hintTextColor : control.flat
                                  && control.highlighted ? control.Material.accentColor : control.highlighted ? control.Material.primaryHighlightedTextColor : control.Material.foreground
    }
}
