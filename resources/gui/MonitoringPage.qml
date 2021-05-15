import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0
import org.onvif.common 1.0
import QuickFuture 1.0
import org.kde.kirigami 2.14 as Kirigami

Kirigami.ScrollablePage {

    title: qsTr("Monitoring")

    ListModel {
        id: fruitModel

        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
    }

    DeviceModel {

        id: deviceModel
    }

    ListView {

        model: deviceModel

        anchors.fill: parent

        delegate: Rectangle {

            color: "transparent"
            border.color: "black"
            border.width: 1
            width: parent.width
            height: 400

            VideoWallPane {
                anchors.fill: parent

                property string streamS: {

                    var deviceInfo = DeviceManager.getDeviceInfo(id)
                    if (deviceInfo.hasMediaService) {
                        stremUrl = deviceInfo.mediaProfiles
                    }
                }
            }
        }

        section.delegate: Kirigami.ListSectionHeader {
            label: "adsf"
            Button {
                text: "Button 1"
            }
            Button {
                text: "Button 2"
            }
        }
    }
}
