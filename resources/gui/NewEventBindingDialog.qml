import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import org.onvif.event 1.0

Dialog {

	id: credentialsDialog

	x: Math.round((window.width - width) / 2)
	y: Math.round(window.height / 6)
	width: Math.round(Math.min(window.width, window.height) / 3 * 2)
	modal: true
	focus: true
	title: qsTr("New Event Binding")

	standardButtons: Dialog.Ok | Dialog.Cancel

	onAccepted: {

		if (bindingName.acceptableInput)
			EventManager.addBinding(bindingName.text, bindingDescription.text)
	}

	contentItem: GridLayout {

		columns: 2

		Label {
			text: qsTr("Name")
		}
		TextField {

			id: bindingName

			activeFocusOnTab: true
			placeholderText: "Binding Name"
			Layout.fillWidth: true
			validator: RegExpValidator {
				regExp: /.*\S.*/
			}
		}

		Label {
			text: qsTr("Description")
		}
		TextField {

			id: bindingDescription

			activeFocusOnTab: true
			placeholderText: "Description"
			Layout.fillWidth: true
		}
	}
}
