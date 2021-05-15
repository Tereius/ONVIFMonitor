import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0

Page {

    title: qsTr("Settings")

    ListModel {

        id: settingsModel

        ListElement {
            label: qsTr("Onvif Devices")
            source: "DeviceSettingsPage.qml"
            iconName: "ic_cast_connected"
        }
        ListElement {
            label: qsTr("Media Profiles")
            source: "MediaProfilesDevicesSettingsPage.qml"
            iconName: "ic_video_library"
        }
        ListElement {
            label: qsTr("Message/Event Handling")
            source: "EventSettingsPage.qml"
            iconName: "ic_appbar_speakerphone"
        }
        ListElement {
            label: qsTr("Other")
            source: ""
            iconName: "ic_settings_applications"
        }
    }

    ScrollView {

        anchors.fill: parent

        ListView {

            anchors.fill: parent

            model: settingsModel

            delegate: ItemDelegate {

                width: parent.width

                text: label
                icon.name: iconName ? iconName : null

                onClicked: {

                    if (source) {
                        window.push(source)
                    }
                }

                rightPadding: moreButton.width

                Icon {
                    id: moreButton
                    name: "ic_chevron_right"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
