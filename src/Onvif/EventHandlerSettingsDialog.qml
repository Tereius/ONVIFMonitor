import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Onvif

Dialog {

    id: settingsDialog

    x: Math.round((window.width - width) / 2)
    y: Math.round(window.height / 6)
    width: Math.round(Math.min(window.width, window.height) / 3 * 2)
    modal: true
    focus: true
    title: qsTr("Event Source")

    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {

    }

    onRejected: {

        settingsDialog.close()
    }

    EventHandlerModel {

        id: eventHandlerModel
    }

    contentItem: GridLayout {

        columns: 2

        Label {
            text: qsTr("Handler")
        }

        ComboBox {

            id: eventHandlerSel

            Layout.fillWidth: true
            model: eventHandlerModel
            textRole: "name"

            onAccepted: {
                hostField.text = eventHandlerModel.get(currentIndex).description
            }
        }

        Text {

            id: hostField

            Layout.columnSpan: 2
        }
    }
}
