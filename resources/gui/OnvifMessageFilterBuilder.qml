import QtQuick 2.0
import QtQuick.Controls 2.3
import "./FilterBuilder"

Item {

	height: childrenRect.height
	width: parent.width

	Column {

		width: parent.width

		BaseFilterComponent {
			label: "AND"
			keys: ["and"]
		}

		BaseFilterComponent {
			label: "OR"
			keys: ["or"]
		}

		DropLine {

			width: parent.width
			height: 5
			acceptedKeys: ["and", "or"]
		}
	}
}
