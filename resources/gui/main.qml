import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import Qt.labs.settings 1.0
import org.onvif.device 1.0
import org.onvif.event 1.0
import org.global 1.0

ApplicationWindow {
	id: window
	width: 360
	height: 520
	visible: true
	title: "ONVIFMonitor"

	signal credentialsProvided(var deviceId)
	property bool busy: false

	function showBusy(show) {
		busy = show
	}

	function push(item, properties, operation) {

		stackView.push(item, properties, operation)
	}

	function showError(errorTitle, errorText) {

		errorDialog.errorTitle = errorTitle
		errorDialog.errorText = errorText
		errorDialog.open()
	}

	Connections {
		target: Window
		onErrorChanged: {
			showError(Window.error.getErrorTitle(), Window.error.getErrorText())
		}
	}

	Component.onCompleted: {
		DeviceManager.initialize()
		EventManager.initialize()
	}

	Settings {
		id: settings
		property string style: "Default"
	}

	Shortcut {
		sequences: ["Esc", "Back"]
		enabled: stackView.depth > 1
		onActivated: {
			stackView.pop()
			listView.currentIndex = -1
		}
	}

	Shortcut {
		sequence: "Menu"
		onActivated: optionsMenu.open()
	}

	header: ToolBar {
		Material.foreground: "white"

		RowLayout {
			spacing: 20
			anchors.fill: parent

			ToolButton {
				enabled: stackView.depth > 1
				visible: stackView.depth > 1
				icon.name: "ic_arrow_back"
				onClicked: {
					if (stackView.depth > 1) {
						stackView.pop()
						listView.currentIndex = -1
					} else {
						drawer.open()
					}
				}
			}

			Label {
				id: titleLabel
				text: listView.currentItem ? listView.currentItem.text : "Gallery"
				font.pixelSize: 20
				elide: Label.ElideRight
				horizontalAlignment: Qt.AlignHCenter
				verticalAlignment: Qt.AlignVCenter
				Layout.fillWidth: true
			}

			ToolButton {
				icon.name: "ic_more_vert"
				onClicked: optionsMenu.open()

				Menu {
					id: optionsMenu
					x: parent.width - width
					transformOrigin: Menu.TopRight

					MenuItem {
						text: qsTr("Settings")
						onTriggered: stackView.push("SettingsPage.qml")
					}
					MenuItem {
						text: qsTr("About")
						onTriggered: aboutDialog.open()
					}
				}
			}
		}

		ProgressBar {

			visible: window.busy
			indeterminate: true
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			background: null
		}
	}

	Drawer {
		id: drawer
		width: Math.min(window.width, window.height) / 3 * 2
		height: window.height
		interactive: stackView.depth === 1

		ListView {
			id: listView

			focus: true
			currentIndex: -1
			anchors.fill: parent

			delegate: ItemDelegate {
				width: parent.width
				text: model.title
				highlighted: ListView.isCurrentItem
				onClicked: {
					listView.currentIndex = index
					stackView.push(model.source)
					drawer.close()
				}
			}

			model: ListModel {
				ListElement {
					title: "BusyIndicator"
					source: "qrc:/pages/BusyIndicatorPage.qml"
				}
				ListElement {
					title: "Button"
					source: "qrc:/pages/ButtonPage.qml"
				}
				ListElement {
					title: "CheckBox"
					source: "qrc:/pages/CheckBoxPage.qml"
				}
				ListElement {
					title: "ComboBox"
					source: "qrc:/pages/ComboBoxPage.qml"
				}
				ListElement {
					title: "DelayButton"
					source: "qrc:/pages/DelayButtonPage.qml"
				}
				ListElement {
					title: "Dial"
					source: "qrc:/pages/DialPage.qml"
				}
				ListElement {
					title: "Dialog"
					source: "qrc:/pages/DialogPage.qml"
				}
				ListElement {
					title: "Delegates"
					source: "qrc:/pages/DelegatePage.qml"
				}
				ListElement {
					title: "Frame"
					source: "qrc:/pages/FramePage.qml"
				}
				ListElement {
					title: "GroupBox"
					source: "qrc:/pages/GroupBoxPage.qml"
				}
				ListElement {
					title: "PageIndicator"
					source: "qrc:/pages/PageIndicatorPage.qml"
				}
				ListElement {
					title: "ProgressBar"
					source: "qrc:/pages/ProgressBarPage.qml"
				}
				ListElement {
					title: "RadioButton"
					source: "qrc:/pages/RadioButtonPage.qml"
				}
				ListElement {
					title: "RangeSlider"
					source: "qrc:/pages/RangeSliderPage.qml"
				}
				ListElement {
					title: "ScrollBar"
					source: "qrc:/pages/ScrollBarPage.qml"
				}
				ListElement {
					title: "ScrollIndicator"
					source: "qrc:/pages/ScrollIndicatorPage.qml"
				}
				ListElement {
					title: "Slider"
					source: "qrc:/pages/SliderPage.qml"
				}
				ListElement {
					title: "SpinBox"
					source: "qrc:/pages/SpinBoxPage.qml"
				}
				ListElement {
					title: "StackView"
					source: "qrc:/pages/StackViewPage.qml"
				}
				ListElement {
					title: "SwipeView"
					source: "qrc:/pages/SwipeViewPage.qml"
				}
				ListElement {
					title: "Switch"
					source: "qrc:/pages/SwitchPage.qml"
				}
				ListElement {
					title: "TabBar"
					source: "qrc:/pages/TabBarPage.qml"
				}
				ListElement {
					title: "TextArea"
					source: "qrc:/pages/TextAreaPage.qml"
				}
				ListElement {
					title: "TextField"
					source: "qrc:/pages/TextFieldPage.qml"
				}
				ListElement {
					title: "ToolTip"
					source: "qrc:/pages/ToolTipPage.qml"
				}
				ListElement {
					title: "Tumbler"
					source: "qrc:/pages/TumblerPage.qml"
				}
			}

			ScrollIndicator.vertical: ScrollIndicator {
			}
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent

		initialItem: Pane {
			anchors.fill: parent
		}
	}

	CredentialsDialog {

		id: credentialsDialog

		Connections {

			target: DeviceManager
			onUnauthorized: {

				credentialsDialog.deviceId = rDeviceId
				credentialsDialog.open()
			}
		}
		onAccepted: {

			DeviceManager.setDeviceCredentials(credentialsDialog.deviceId,
											   credentialsDialog.user,
											   credentialsDialog.pwd,
											   credentialsDialog.save)
			window.credentialsProvided(credentialsDialog.deviceId)
		}
	}

	Dialog {

		property alias errorText: errorLabel.text
		property alias errorTitle: errorDialog.title

		id: errorDialog
		modal: true
		focus: true
		title: qsTr("Error")
		x: (window.width - width) / 2
		y: window.height / 6
		width: Math.min(window.width, window.height) / 3 * 2
		contentHeight: errorColumn.height

		standardButtons: Dialog.Ok

		Column {
			id: errorColumn
			spacing: 20

			Label {
				id: errorLabel
				width: aboutDialog.availableWidth
				wrapMode: Label.Wrap
				font.pixelSize: 12
			}
		}
	}

	Dialog {

		id: aboutDialog
		modal: true
		focus: true
		title: "About"
		x: (window.width - width) / 2
		y: window.height / 6
		width: Math.min(window.width, window.height) / 3 * 2
		contentHeight: aboutColumn.height

		Column {
			id: aboutColumn
			spacing: 20

			Label {
				width: aboutDialog.availableWidth
				text: "The Qt Quick Controls 2 module delivers the next generation user interface controls based on Qt Quick."
				wrapMode: Label.Wrap
				font.pixelSize: 12
			}

			Label {
				width: aboutDialog.availableWidth
				text: "In comparison to the desktop-oriented Qt Quick Controls 1, Qt Quick Controls 2 " + "are an order of magnitude simpler, lighter and faster, and are primarily targeted "
					  + "towards embedded and mobile platforms."
				wrapMode: Label.Wrap
				font.pixelSize: 12
			}
		}
	}

	BusyPopup {

		id: busyPopup
		Connections {
			target: Window
			onModalBusyChanged: {
				if (Window.modalBusy === false) {
					busyPopup.close()
				}
			}
		}

		Timer {
			interval: 500
			repeat: false
			running: Window.modalBusy
			onTriggered: busyPopup.open()
		}
	}
}
