import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0
import org.kde.kirigami 2.14 as Kirigami

Kirigami.ScrollablePage {

    title: "Devices"

    ListView {

        model: devicesModel

        delegate: ItemDelegate {

            width: parent.width

            text: name

            onClicked: {
                window.push("MediaProfilesListPage.qml", {
                                "deviceId": deviceId
                            })
            }

            rightPadding: moreIcon.width

            Icon {
                id: moreIcon
                name: "ic_chevron_right"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    DeviceModel {
        id: devicesModel
    }
}
