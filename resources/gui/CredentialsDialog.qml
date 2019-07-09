import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

Dialog {

	property alias user: userTextField.text
	property alias pwd: pwdTextField.text
	property alias save: saveCheckBox.checked
	property url endpoint
	property var deviceId

	id: credentialsDialog

	x: Math.round((window.width - width) / 2)
	y: Math.round(window.height / 6)
	width: Math.round(Math.min(window.width, window.height) / 3 * 2)
	modal: true
	focus: true
	title: qsTr("Authentification required")

	standardButtons: Dialog.Ok | Dialog.Cancel

	onAccepted: {

		credentialsDialog.close()
	}

	onRejected: {

		credentialsDialog.close()
	}

	onAboutToShow: {

		userTextField.clear()
		pwdTextField.clear()
		save = false
	}

	contentItem: GridLayout {

		columns: 2

		Label {
			text: qsTr("User")
		}
		TextField {

			id: userTextField

			activeFocusOnTab: true
			placeholderText: "Name"
			Layout.fillWidth: true
		}

		Label {
			text: qsTr("Password")
		}
		TextField {

			id: pwdTextField

			echoMode: TextInput.PasswordEchoOnEdit

			activeFocusOnTab: true
			placeholderText: "*********"
			Layout.fillWidth: true
		}

		Label {
			text: qsTr("Remember")
		}
		CheckBox {

			id: saveCheckBox

			checked: false
			ToolTip.delay: 1000
			ToolTip.timeout: 5000
			ToolTip.visible: hovered
			ToolTip.text: qsTr("Save the credentials")
		}
	}
}
