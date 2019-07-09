import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtAV 1.7

Pane {

	property alias stremUrl: player.source

	background: Rectangle {

		anchors.fill: parent
		color: "#000000"
	}

	Rectangle {
		anchors.fill: parent
		VideoOutput2 {
			anchors.fill: parent
			source: player
		}
		AVPlayer {
			id: player
		}
		MouseArea {
			anchors.fill: parent
			onClicked: player.play()
		}
	}
}
