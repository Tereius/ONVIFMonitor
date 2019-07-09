import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3

Item {

	id: control

	property alias name: iconRoot.icon.name
	property alias color: iconRoot.icon.color

	width: 38
	height: 38

	Button {
		id: test
		visible: false
	}

	Button {

		id: iconRoot
		enabled: false

		anchors.fill: parent

		icon.color: test.palette.buttonText // TODO: ugly workaround
		icon.width: control.width
		icon.height: control.height

		display: AbstractButton.IconOnly

		background: Rectangle {

			color: "transparent"
		}
	}
}
