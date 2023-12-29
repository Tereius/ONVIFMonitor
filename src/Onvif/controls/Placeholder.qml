import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import MaterialRally as Rally

Rally.GroupBox {

    id: group

    property alias text: label.text
    property alias icon: icon.icon
    property alias showButton: takeMeThereButton.visible
    property alias buttonText: takeMeThereButton.text
    property alias buttonIcon: takeMeThereButton.icon

    signal clicked

    width: Math.min(parent.width, 300)
    height: Math.min(parent.height, Math.max(column.implicitHeight + 40, 150))
    padding: 10
    Material.roundedScale: Material.MediumScale

    Column {

        id: column
        spacing: 20
        anchors.centerIn: parent

        Rally.Icon {
            id: icon
            icon.width: 36
            icon.height: 36
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label
            width: group.availableWidth
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 16
        }

        Button {
            id: takeMeThereButton
            text: qsTr("Take me there")
            flat: true
            visible: false
            anchors.horizontalCenter: parent.horizontalCenter
            LayoutMirroring.enabled: true
            icon.name: "chevron-right"
            onClicked: {
                group.clicked()
            }
        }
    }
}
