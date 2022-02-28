import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

T.ComboBox {

    id: control
    selectTextByMouse: true

    property color placeholderTextColor: Material.secondaryTextColor
    property string placeholderText: ""
    property int length: control.displayText.length
    //topPadding: 22
    //bottomPadding: 5
    leftPadding: 0
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    indicator: ColorImage {
        x: control.mirrored ? control.padding : control.width - width - control.padding
        y: ((!control.editable && control.hovered)
            || indicatorArea.containsMouse
            || control.down ? 5 : 0) + control.topPadding + (control.availableHeight - height) / 2
        height: parent.height
        fillMode: Image.PreserveAspectFit
        color: {
            if (control.enabled) {
                if (control.editable) {
                    return indicatorArea.containsMouse ? control.Material.accentColor : control.Material.foreground
                } else {

                    return control.hovered ? control.Material.accentColor : control.Material.foreground
                }
            }
            return control.Material.hintTextColor
        }
        source: "qrc:/qt-project.org/imports/QtQuick/Controls.2/Material/images/drop-indicator.png"

        Ripple {
            visible: control.editable
            y: -parent.y
            clipRadius: 1
            width: parent.width
            height: parent.height
            pressed: control.pressed
            active: true
            color: control.flat
                   && control.highlighted ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }

        Behavior on y {
            SmoothedAnimation {
                duration: 150
                velocity: -1
            }
        }

        MouseArea {
            id: indicatorArea
            anchors.fill: parent
            hoverEnabled: control.hoverEnabled
            acceptedButtons: Qt.NoButton
        }
    }

    contentItem: TextField {

        text: control.editable ? control.editText : control.displayText

        rightPadding: 0
        implicitWidth: 0

        enabled: control.enabled
        autoScroll: control.editable
        readOnly: control.down || !control.editable
        inputMethodHints: control.inputMethodHints
        validator: control.validator

        font: control.font
        color: control.enabled ? control.Material.foreground : control.Material.hintTextColor
        selectionColor: control.Material.accentColor
        selectedTextColor: control.Material.primaryHighlightedTextColor
        verticalAlignment: Text.AlignVCenter

        placeholderText: control.placeholderText
        placeholderTextColor: control.placeholderTextColor

        cursorDelegate: CursorDelegate {}

        onPressed: {
            if (!control.editable) {
                event.accepted = false
            }
        }
    }

    onHoveredChanged: {
        control.background.manualHover = control.hovered
    }

    background: control.contentItem.background
}
