import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtGraphicalEffects 1.0

Item {

	id: conrol

	property alias fillMode: image.fillMode
	property bool finished: image.status === Image.Ready ? true : false
	property var profileId

	onProfileIdChanged: {

		load(profileId)
	}

	function load(profileId, timestamp) {

		console.debug("Camera snapshot requested for profile: " + profileId)
		d.profileId = profileId
		if (!timestamp) {
			timestamp = new Date()
		}
		if (!profileId) {
			profileId = new ProfileId()
			console.warn("No profile id given")
		}

		image.source = "image://profile/" + profileId.getDeviceId(
					) + "/" + profileId.getProfileToken() + "/" + timestamp
	}

	QtObject {

		id: d
		property var profileId
	}

	Image {

		id: image

		anchors.fill: parent

		asynchronous: false
		cache: true
		sourceSize.width: 1280
		sourceSize.height: 720

		Rectangle {

			anchors.fill: parent

			color: "#EEEEEE"

			visible: image.status === Image.Ready ? false : true

			Icon {

				id: loadingIcon

				name: "ic_linked_camera"

				width: Math.round(Math.min(parent.width, parent.height) / 3.0)
				height: width

				visible: image.status === Image.Loading ? true : false

				anchors.centerIn: parent

				SequentialAnimation {

					running: true
					loops: Animation.Infinite
					OpacityAnimator {

						target: loadingIcon
						from: 0
						to: 1
						duration: 1000
						easing.type: Easing.InOutSine
					}

					OpacityAnimator {

						target: loadingIcon
						from: 1
						to: 0
						duration: 1000
						easing.type: Easing.InOutSine
					}
				}
			}

			Icon {

				id: loadingFailedIcon

				name: "ic_warning"

				width: Math.round(Math.min(parent.width, parent.height) / 3.0)
				height: width

				visible: image.status === Image.Error ? true : false

				anchors.centerIn: parent
			}
		}

		WRoundButton {

			anchors.right: parent.right
			anchors.margins: 10
			anchors.bottom: parent.bottom

			visible: image.status === Image.Loading ? false : true
			icon.name: "ic_refresh"

			onClicked: {

				conrol.load(d.profileId)
			}
		}
	}
}
