import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3

Button {

	property bool fillIcon: false

	padding: 5

	display: AbstractButton.IconOnly

	icon.width: fillIcon ? width : d.originalIconWidth
	icon.height: fillIcon ? height : d.originalIconHeight
	opacity: hovered ? 1.0 : 0.5

	QtObject {

		id: d
		property int originalIconWidth: icon.width
		property int originalIconHeight: icon.height
	}

	background: Rectangle {
		width: parent.width
		height: parent.height
		implicitHeight: 40
		implicitWidth: 40
		color: "white"
		radius: width * 0.5
		border.color: "black"
		opacity: parent.hovered ? 0.75 : 1
		border.width: parent.hovered ? 2 : 0
	}
}
