import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3

Dialog {

	property var deviceInfo

	id: infoDialog
	x: Math.round((window.width - width) / 2)
	y: Math.round(window.height / 6)
	width: Math.round(Math.min(window.width, window.height) / 3 * 2)
	modal: true
	focus: true
	title: qsTr("Device info")

	standardButtons: Dialog.Close
	onAccepted: {
		infoDialog.close()
	}
	onRejected: {
		infoDialog.close()
	}

	contentItem: ScrollView {

		clip: true

		GridLayout {

			columns: 2

			Label {
				text: qsTr("Name")
			}
			Text {
				text: deviceInfo.deviceName
				wrapMode: Text.Wrapp
			}
			Label {
				text: qsTr("Id")
			}
			Text {
				text: deviceInfo.deviceId
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Host")
			}
			Text {
				text: deviceInfo.host
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Port")
			}
			Text {
				text: deviceInfo.port
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Endpoint")
			}
			Text {
				text: deviceInfo.deviceEndpoint
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Endpoint ref")
			}
			Text {
				text: deviceInfo.endpointReference
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Firmware")
			}
			Text {
				text: deviceInfo.firmwareVersion
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Hardware")
			}
			Text {
				text: deviceInfo.hardwareId
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Manufacturer")
			}
			Text {
				text: deviceInfo.manufacturer
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Model")
			}
			Text {
				text: deviceInfo.model
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Serial Nr")
			}
			Text {
				text: deviceInfo.serialNumber
				wrapMode: Text.Wrap
			}
			Label {
				text: qsTr("Time offset")
			}
			Text {
				text: deviceInfo.dateTimeOffset
				wrapMode: Text.Wrap
			}
		}
	}
}
