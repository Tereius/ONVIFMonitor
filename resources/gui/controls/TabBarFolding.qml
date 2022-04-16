import QtQuick 2.12
import QtQuick.Controls 2.12

Row {

    id: control
    leftPadding: 0
    rightPadding: 0
    padding: 0

    property int currentIndex: 0

    onCurrentIndexChanged: {
        const button = group.buttons[control.currentIndex]
        if (button) {
            button.checked = true
        }
    }

    readonly property int count: group.buttons.length

    function setCurrentIndex(index) {

        if (index >= 0 && index < group.buttons.length) {
            group.buttons[index].checked = true
        }
    }

    // This signal is emitted when a TabButton is interactively toggled
    signal indexSelected(int index)

    spacing: {

        let overallw = 0
        for (var key in group.buttons) {
            let button = group.buttons[key]
            overallw += button.width
        }

        const diff = control.width - overallw - control.leftPadding - control.rightPadding
        const numButtons = group.buttons.length
        if (diff > 0 && numButtons > 1) {
            return diff / (numButtons - 1)
        }
        return 0
    }

    ButtonGroup {
        id: group
        exclusive: true
        buttons: control.children

        // The max. expanded width of the row of the buttons
        property real maxWidth: 0

        function updateMaxWidth() {

            let maxw = 0
            for (var key in group.buttons) {
                let button = buttons[key]
                if (button.hasOwnProperty("proposedWidth")) {
                    maxw = Math.max(button.proposedWidth, maxw)
                }
            }
            group.maxWidth = maxw
        }

        function buttonToggled() {

            let counter = 0
            let currentButton = group.checkedButton
            for (var key in group.buttons) {
                let button = group.buttons[key]
                if (button === currentButton) {
                    break
                }
                counter++
            }
            control.indexSelected(counter)
        }

        onButtonsChanged: {
            for (var key in group.buttons) {
                let button = buttons[key]
                if (button.hasOwnProperty("proposedWidth")) {
                    button.onProposedWidthChanged.disconnect(
                                group.updateMaxWidth)
                    button.onProposedWidthChanged.connect(group.updateMaxWidth)
                    if (button.hasOwnProperty("expandWidth")) {
                        button.expandWidth = Qt.binding(function () {
                            return group.maxWidth
                        })
                    }
                    button.onToggled.disconnect(group.buttonToggled)
                    button.onToggled.connect(group.buttonToggled)
                }
            }
            group.updateMaxWidth()
        }
    }
}
