import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import org.onvif.media 1.0
import org.onvif.common 1.0
import QtAV 1.7

Page {

	property var profileId
	property var futureResult: MediaManager.getMediaProfile(profileId)
	property var profileInfo: futureResult.result

	id: page

	title: qsTr("Profile Settings")

	Component.onCompleted: {


		//proxyImage.load(profileId)
	}

	ScrollView {

		id: scrollView

		anchors.fill: parent

		Column {

			CameraImage {

				id: proxyImage
				profileId: page.profileId

				width: page.width
				height: page.width * 9.0 / 16.0

				WRoundButton {

					id: loadingFailedIcon

					fillIcon: true
					icon.name: "ic_play_arrow"

					width: Math.round(Math.min(parent.width,
											   parent.height) / 3.0)
					height: width

					visible: proxyImage.finished ? true : false

					anchors.centerIn: parent

					onClicked: {
						loadingFailedIcon.visible = false
						var obj = qtav.createObject(proxyImage)
						obj.source = MediaManager.getStreamUrl(profileId)
						obj.play()
					}
				}

				Component {

					id: qtav

					Rectangle {

						property alias source: player.source

						function play() {
							player.play()
						}

						anchors.fill: parent
						VideoOutput2 {
							anchors.fill: parent
							source: player
						}

						AVPlayer {
							id: player
						}
					}
				}
			}

			RowLayout {

				width: page.width
				height: 50

				spacing: 10

				Label {

					Layout.fillWidth: true

					elide: Text.ElideRight
					Material.foreground: Material.accent
					Universal.foreground: Universal.accent
					text: qsTr("Profile settings")
					font.pixelSize: 15
				}

				Button {

					text: qsTr("Refresh")
					icon.name: "ic_refresh"
					onClicked: {

						page.profileId = page.profileId
					}
				}
			}

			ItemDelegate {

				width: page.width

				contentItem: Column {

					Label {
						width: parent.width
						text: "fixed"

						Material.foreground: Material.accent
						Universal.foreground: Universal.accent
					}
					Text {
						width: parent.width
						text: profileInfo.fixed
						wrapMode: Text.WrapAtWordBoundaryOrAnywhere
					}
				}
			}
		}
	}
}
