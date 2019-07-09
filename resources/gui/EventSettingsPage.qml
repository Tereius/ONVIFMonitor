import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import org.onvif.device 1.0
import org.onvif.event 1.0

Page {

	id: page

	property var deviceId

	title: qsTr("Event Handling")

	EventBindingModel {
		id: eventBindingModel
	}

	ExpandableListView {

		id: bindingListView

		height: contentHeight
		width: parent.width
		interactive: false

		model: eventBindingModel
		delegate: ListItem {

			id: bindingItem

			width: parent.width

			label: name
			text: description

			content: Item {

				height: row.height
				width: parent.width

				RowLayout {

					id: row
					width: parent.width
					anchors.centerIn: parent

					RoundButton {
						text: qsTr("Source")

						onClicked: {
							var component = Qt.createComponent(
										"EventPropertiesDialog.qml")
							var dialog = component.createObject(page, {
																	bindingId: bindingId,
																	handler: false
																})
							dialog.onClosed.connect(function () {
								dialog.destroy()
							})
							dialog.open()
						}

						background: rectangel

						RectangularGlow {
							visible: parent.hovered
							z: -2
							color: hasSource ? Universal.color(
												   Universal.Green) : Universal.color(
												   Universal.Red)
							anchors.fill: parent
							glowRadius: 5
							spread: 0
							cornerRadius: rectangel.radius
						}

						Rectangle {
							id: rectangel
							radius: height / 2
							border.width: 2
							border.color: hasSource ? Universal.color(
														  Universal.Green) : Universal.color(
														  Universal.Red)
							color: "#33000000"
						}
					}

					Rectangle {
						width: 75
						height: 2
						color: "black"
						anchors.verticalCenter: row.verticalCenter
						Layout.minimumWidth: 5
						Layout.fillWidth: true
					}

					RoundButton {

						implicitHeight: 50
						implicitWidth: 50

						anchors.verticalCenter: row.verticalCenter

						icon.name: "ic_appbar_link"
						onClicked: {

							var result = EventManager.triggerHandler(bindingId)
							if (!result.isSuccess())
								window.showError("error", result.toString())
						}

						PingAnimation {
							id: pingAnim
							anchors.fill: parent

							Connections {
								enabled: bindingItem.highlighted
								target: EventManager
								onEventOccured: {
									if (rBindingId === bindingId)
										pingAnim.ping()
								}
							}
						}
					}

					Rectangle {
						width: 75
						height: 2
						color: "black"
						anchors.verticalCenter: row.verticalCenter
						Layout.minimumWidth: 5
						Layout.fillWidth: true
					}

					RoundButton {
						text: qsTr("Handler")

						onClicked: {
							var component = Qt.createComponent(
										"EventPropertiesDialog.qml")
							var dialog = component.createObject(page, {
																	bindingId: bindingId,
																	handler: true
																})
							dialog.onClosed.connect(function () {
								dialog.destroy()
							})
							dialog.open()
						}
					}
				}
			}
		}

		header: RowLayout {

			width: parent.width
			height: 50

			spacing: 10

			Label {

				Layout.fillWidth: true

				elide: Text.ElideRight
				Material.foreground: Material.accent
				Universal.foreground: Universal.accent
				text: qsTr("Events")
				font.pixelSize: 15
			}

			Button {

				text: qsTr("Add")
				icon.name: "ic_add"

				onClicked: {

					newEventBindingDialog.open()
				}
			}
		}
	}

	NewEventBindingDialog {

		id: newEventBindingDialog
	}
}
