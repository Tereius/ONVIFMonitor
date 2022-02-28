import QtQuick 2.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.14 as Kirigami
import "controls" as Controls

Kirigami.Page {

    Controls.GroupBox {

        id: group
        width: Math.min(parent.width, 300)
        height: Math.min(parent.height, 150)
        anchors.centerIn: parent
        padding: 10

        Column {

            spacing: 20
            anchors.centerIn: parent

            Controls.Icon {
                icon.name: "cctv-off"
                icon.width: 36
                icon.height: 36
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: qsTr("Not yet implemented")
                width: group.availableWidth
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 16
            }
        }
    }
}
