import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

Item {

    id: control

    implicitHeight: iconLabel.implicitHeight
    implicitWidth: iconLabel.implicitWidth

    property alias icon: iconLabel.icon
    property bool flat: true
    property bool highlighted: false

    icon.width: 20
    icon.height: 20
    icon.color: !control.enabled ? Material.hintTextColor : control.flat
                                   && control.highlighted ? Material.accentColor : control.highlighted ? Material.primaryHighlightedTextColor : Material.foreground

    IconLabel {
        id: iconLabel
        display: AbstractButton.IconOnly
    }
}
