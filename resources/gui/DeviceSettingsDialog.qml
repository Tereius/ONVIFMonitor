import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0

Dialog {

	id: credentialsDialog

	property var deviceId
	property bool isNameEdited: false
	property bool isHostEdited: false

	onDeviceIdChanged: {

		isNameEdited = isHostEdited = false
		var di = DeviceManager.getDeviceInfo(deviceId)
		deviceNameField.text = di.deviceName
		hostField.text = di.deviceEndpoint
	}

	x: Math.round((window.width - width) / 2)
	y: Math.round(window.height / 6)
	width: Math.round(Math.min(window.width, window.height) / 3 * 2)
	modal: true
	focus: true
	title: qsTr("Device Settings")

	standardButtons: Dialog.Ok | Dialog.Cancel

	onAccepted: {

		if (isHostEdited) {
			var result = DeviceManager.setDeviceHost(deviceId, hostField.text)
			if (result.isFault()) {
				window.showError(qsTr("Host invalid"),
								 qsTr("Couldn't set the new host: ") + result)
				return
			}
		}
		if (isNameEdited) {
			DeviceManager.renameDevice(deviceId, deviceNameField.text)
		}
		isNameEdited = isHostEdited = false
		//credentialsDialog.close()
	}

	onRejected: {

		credentialsDialog.close()
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

		Label {
			text: ""
		}
		DelayButton {

			Layout.minimumWidth: 0
			text: "Delete Device"
			onClicked: {

				window.showError(
							qsTr("Warning"), qsTr(
								"Please hold the button for 3 seconds to delete the device. All settings will be lost."))
			}
			onActivated: {

				DeviceManager.removeDevice(deviceId)
				credentialsDialog.close()
			}
		}
	}
}
