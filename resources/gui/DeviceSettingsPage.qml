import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0

Page {

	id: settingsPage
	title: qsTr("Devices")

	Component.onCompleted: {

		deviceDiscoveryModel.start()
		timer.start()
	}

	Component.onDestruction: {

		timer.stop()
		deviceDiscoveryModel.stop()
	}

	Timer {

		id: timer
		interval: 15000
		running: false
		repeat: false
	}

	ScrollView {

		id: scrollView

		anchors.fill: parent
		padding: 10

		Column {

			ListView {

				height: contentHeight
				width: scrollView.availableWidth

				interactive: false

				header: RowLayout {

					width: parent.width
					height: 50

					spacing: 10

					Label {

						Layout.fillWidth: true

						elide: Text.ElideRight
						Material.foreground: Material.accent
						Universal.foreground: Universal.accent
						text: qsTr("Configured devices")
						font.pixelSize: 15
					}

					Button {

						text: qsTr("Add")
						icon.name: "ic_add"
						onClicked: {

							newDeviceDialog.open()
						}
					}
				}

				footer: Text {

					width: parent.width
					height: parent.count == 0 ? 50 : 0
					visible: parent.count == 0 ? true : false
					elide: Text.ElideRight
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					wrapMode: Text.WordWrap

					text: qsTr("Currently there are no configured devices")
				}

				onCountChanged: {
					console.log("count changed" + count)
				}

				model: deviceModel
				delegate: ItemDelegate {

					width: parent.width
					height: 50

					text: name

					onClicked: {

						window.push("DeviceInfoPage.qml", {
										deviceId: deviceId
									})
					}
					rightPadding: settingsButton.width + errorButton.width

					ToolButton {
						id: settingsButton
						icon.name: "ic_settings"
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						onClicked: {
							deviceSettingsDialog.deviceId = deviceId
							deviceSettingsDialog.open()
						}
					}
					ToolButton {
						id: errorButton
						visible: !initialized
						icon.name: "ic_warning"
						icon.color: "#FCE100"
						anchors.right: settingsButton.left
						anchors.verticalCenter: parent.verticalCenter
						onClicked: {
							window.showError(
										qsTr("Device initialization error"),
										error)
						}
					}
				}
			}

			ListView {

				height: contentHeight
				width: scrollView.availableWidth

				interactive: false

				header: RowLayout {

					width: parent.width
					height: 50

					spacing: 10

					Label {

						Layout.fillWidth: true

						elide: Text.ElideRight
						Material.foreground: Material.accent
						Universal.foreground: Universal.accent
						text: qsTr("Available devices")
						font.pixelSize: 15
					}

					Button {

						id: refreshButton
						text: "Search"
						icon.name: "ic_refresh"
						visible: false
						onClicked: {
							visible = false
							discoveryProgress.visible = true
							deviceDiscoveryModel.reset()
							deviceDiscoveryModel.start()
							timer.start()
						}
						Connections {
							target: timer
							onTriggered: {
								refreshButton.visible = true
								discoveryProgress.visible = false
								deviceDiscoveryModel.stop()
							}
						}
					}

					ProgressBar {

						anchors.top: parent.bottom
						anchors.left: parent.left
						anchors.right: parent.right

						id: discoveryProgress
						visible: true
						indeterminate: true
					}
				}

				footer: Text {

					id: serchDevicesFooter

					width: parent.width
					height: parent.count == 0 ? 50 : 0

					visible: parent.count == 0

					elide: Text.ElideRight
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					wrapMode: Text.WordWrap

					text: qsTr("No devices were found")
				}

				onCountChanged: {
					if (count == 0) {


						//serchDevicesFooter.visible = true
						//footerItem.height = 50
					} else {


						//serchDevicesFooter.visible = false
						//footerItem.height = 0
					}
				}

				model: deviceDiscoveryModel
				delegate: ItemDelegate {

					width: parent.width
					height: 50

					icon.name: isNew ? "ic_fiber_new" : null
					text: host

					onClicked: {


						//infoDialog.deviceInfo = DeviceManager.getDeviceInfo(
						//			endpoint)
						//infoDialog.open()
					}

					rightPadding: addButton.width
					ToolButton {
						id: addButton
						icon.name: "ic_add"
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						enabled: isNew
						onClicked: {
							DeviceManager.addDevice(endpoint)
						}
					}
				}
			}
		}
	}

	Connections {

		target: window
		onCredentialsProvided: {
			DeviceManager.reinitializeDevice(deviceId)
		}
	}

	DeviceDiscoverModel {

		id: deviceDiscoveryModel
	}

	DeviceModel {

		id: deviceModel
	}

	DeviceSettingsDialog {

		id: deviceSettingsDialog
	}

	NewDeviceDialog {

		id: newDeviceDialog
	}
}
