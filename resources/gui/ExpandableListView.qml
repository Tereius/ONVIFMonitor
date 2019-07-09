import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3

ListView {

	id: component

	property Item expandedItem
	property int expandedIndex

	currentIndex: -1

	highlight: highlight
	highlightFollowsCurrentItem: false
	focus: true

	Keys.onUpPressed: {
		if (currentIndex === -1) {
			if (expandedItem)
				currentIndex = expandedIndex
			else
				currentIndex = 0
		} else if (currentIndex > 0)
			component.decrementCurrentIndex()
	}

	Keys.onDownPressed: {
		if (currentIndex === -1) {
			if (expandedItem)
				currentIndex = expandedIndex
			else
				currentIndex = 0
		} else if (currentIndex < count - 1)
			component.incrementCurrentIndex()
	}

	onFocusChanged: {
		if (!focus)
			currentIndex = -1
	}

	Component {
		id: highlight
		Rectangle {
			color: "transparent"
			border.color: "black"
			border.width: 3
			visible: component.currentItem ? true : false
			x: component.currentItem ? component.currentItem.x : 0
			y: component.currentItem ? component.currentItem.y : 0
			width: component.currentItem ? component.currentItem.width : 0
			height: component.currentItem ? component.currentItem.height : 0
		}
	}
}
