import QtQuick 2.0

Item {

	id: component
	height: 40
	width: 40

	property int duration: 300
	property color color: "black"
	property int lineStrength: width * 0.05

	function ping() {
		animation.start()
	}

	Rectangle {
		id: rect
		width: 0
		height: 0
		anchors.centerIn: component

		color: "transparent"
		radius: width * 0.5
		border.color: component.color
		border.width: lineStrength

		ParallelAnimation {

			id: animation

			alwaysRunToEnd: true
			running: false

			NumberAnimation {
				target: rect
				property: "width"
				duration: component.duration
				easing.type: Easing.Linear
				from: 0
				to: component.width
			}

			NumberAnimation {
				target: rect
				properties: "height"
				duration: component.duration
				easing.type: Easing.Linear
				from: 0
				to: component.height
			}

			NumberAnimation {
				target: rect
				properties: "opacity"
				duration: component.duration
				easing.type: Easing.InCubic
				from: 1
				to: 0
			}

			NumberAnimation {
				target: rect
				properties: "border.width"
				duration: component.duration
				easing.type: Easing.InCubic
				from: lineStrength
				to: 0
			}
		}
	}
}
