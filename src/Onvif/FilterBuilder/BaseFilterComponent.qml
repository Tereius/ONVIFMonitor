import QtQuick 2.7
import QtQuick.Controls 2.3

Rectangle {

	id: filterBase

	property var keys: []
	property string label: "unnamed"

	border.width: 2
	border.color: "black"
	color: "red"

	implicitWidth: label.contentWidth
	implicitHeight: label.contentHeight

	Label {

		id: label

		text: filterBase.label
		anchors.centerIn: parent
	}

	MouseArea {
		id: dragArea
		anchors.fill: parent

		property var dragObj

		onPressed: {

			parent.grabToImage(function (result) {

				dragObj = dragImage.createObject(filterBase, {
													 source: result.url
												 })
				dragObj.x = mouse.x
				dragObj.y = mouse.y

				dragArea.drag.target = dragObj
			})
		}

		onReleased: {

			if (dragObj && dragObj.Drag.target) {
				component.createObject(dragObj.Drag.target.parent)
			}

			dragArea.drag.target = null

			if (dragObj)
				dragObj.destroy()
		}
	}

	Component {

		id: dragImage

		Image {

			id: img

			Drag.active: dragArea.drag.active
			Drag.hotSpot.x: 0
			Drag.hotSpot.y: 0
			Drag.keys: filterBase.keys
		}
	}

	Component {

		id: dropLine

		DropLine {
		}
	}

	Component {

		id: component

		Rectangle {

			width: parent.width
			height: childrenRect.height

			border.width: 2
			border.color: "blue"
			color: "transparent"

			Column {

				width: parent.width
				height: childrenRect.height
				padding: 10

				Label {
					text: filterBase.label + " {"
				}

				Column {

					width: parent.width
					height: childrenRect.height

					DropLine {
						width: parent.width
						height: 3
					}
				}

				Label {
					text: "}"
				}
			}
		}
	}
}
