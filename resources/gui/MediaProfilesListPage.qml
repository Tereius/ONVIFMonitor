import QtQuick 2.10
import QtQuick.Controls 2.3
import org.onvif.media 1.0

Page {

    property alias deviceId: mediaProfilesModel.deviceId

    title: qsTr("Profile Settings")

    ScrollView {

        anchors.fill: parent

        ListView {

            anchors.fill: parent

            model: mediaProfilesModel

            delegate: ItemDelegate {

                width: parent.width

                text: name

                rightPadding: moreIcon.width

                onClicked: {
                    window.push("MediaProfileSettingsPage.qml", {
                                    "profileId": profileId
                                })
                }

                Icon {
                    id: moreIcon
                    name: "ic_chevron_right"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    MediaProfilesModel {
        id: mediaProfilesModel
    }
}
