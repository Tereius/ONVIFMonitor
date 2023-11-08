import QtQuick 2.0

Rectangle {

	property var acceptedKeys: []
	color: dropArea.containsDrag ? "yellow" : "blue"

	DropArea {

		id: dropArea

		width: parent.width
		height: 20
		anchors.verticalCenter: parent.verticalCenter

		keys: acceptedKeys
	}
}
