import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0

Dialog {

	id: credentialsDialog

	x: Math.round((window.width - width) / 2)
	y: Math.round(window.height / 6)
	width: Math.round(Math.min(window.width, window.height) / 3 * 2)
	modal: true
	focus: true
	title: qsTr("New Device")

	standardButtons: Dialog.Ok | Dialog.Cancel

	onAccepted: {

		DeviceManager.addDevice(hostField.text, deviceNameField.text)
	}

	contentItem: GridLayout {

		columns: 2

		Label {
			text: qsTr("Name")
		}
		TextField {

			id: deviceNameField

			activeFocusOnTab: true
			placeholderText: "Device Name"
			Layout.fillWidth: true
			onEditingFinished: isNameEdited = true
		}

		Label {
			text: qsTr("Host")
		}
		TextField {

			id: hostField

			activeFocusOnTab: true
			placeholderText: "Device Host"
			Layout.fillWidth: true
			onEditingFinished: isHostEdited = true
		}
	}
}
