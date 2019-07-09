import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3

ItemDelegate {

	id: component

	property string label
	property alias moreText: component.text
	property alias buttons: buttonContainer.children
	property alias content: moreContent.children
	property bool showMore: false
	property alias expanded: component.highlighted

	readonly property var view: ListView.view

	activeFocusOnTab: true

	Connections {
		target: view
		onExpandedItemChanged: {
			if (view.expandedItem !== component) {
				expanded = false
			}
		}
	}

	// Only used to detect clicks to end keyboard navigation
	Item {

		z: -1
		anchors.fill: component

		MouseArea {
			anchors.fill: parent
			propagateComposedEvents: true
			onClicked: {
				parent.parent.clicked()
				view.currentIndex = -1
			}
		}
	}

	onClicked: {
		component.highlighted = !component.highlighted
		if (!component.highlighted) {
			ListView.view.expandedItem = null
			ListView.view.expandedIndex = -1
		}
	}

	onExpandedChanged: {
		if (expanded) {
			if (ListView.view && ListView.view.expandedItem !== undefined
					&& ListView.view.expandedItem !== component) {
				ListView.view.expandedItem = component
				ListView.view.expandedIndex = index
			}
		}
	}

	contentItem: Column {

		width: parent.width
		spacing: 4

		Label {
			width: parent.width
			text: component.label
			elide: Text.ElideRight

			Material.foreground: Material.accent
			Universal.foreground: Universal.accent
		}

		Text {
			width: parent.width
			text: component.moreText
			wrapMode: Text.WrapAtWordBoundaryOrAnywhere
			visible: text.length > 0
		}

		Item {
			id: moreContent
			width: parent.width
			height: childrenRect.height
			visible: component.expanded
		}
	}

	rightPadding: buttonContainer.width + moreIcon.width

	Icon {
		id: moreIcon
		name: "ic_chevron_right"
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		visible: component.showMore
	}

	Item {
		id: buttonContainer

		anchors.right: moreIcon.left
		anchors.verticalCenter: parent.verticalCenter
	}
}
