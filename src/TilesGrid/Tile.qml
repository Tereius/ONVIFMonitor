import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {

    id: tile

    property int rowSpan: 1
    property int columnSpan: 1
    readonly property bool dragActive: mouseArea.drag.active
    property int modelIndex: index != null ? index : 0
    property alias draggable: mouseArea.enabled

    color: "white"
    border.width: 3
    border.color: "blue"
    implicitHeight: 50 * tile.rowSpan
    implicitWidth: 50 * tile.columnSpan

    Image {

        id: dragDummy
        width: tile.width
        height: tile.height
        cache: false

        //opacity: mouseArea.drag.active ? 0.8 : 0

        //parent: mouseArea.drag.active ? ApplicationWindow.contentItem : tile
        Drag.keys: ["tile"]
        Drag.active: mouseArea.drag.active
        Drag.dragType: Drag.Internal
        Drag.source: tile
        Drag.hotSpot.x: tile.width / tile.columnSpan / 2
        Drag.hotSpot.y: tile.height / tile.rowSpan / 2

        onStatusChanged: status => {
                             if (status === Image.Ready) {
                                 tile.opacity = 0.2
                             } else {
                                 tile.opacity = 1
                             }
                         }

        function reparent(newParent) {

            let pos = dragDummy.parent.mapToItem(tile, dragDummy.x, dragDummy.y)
            animX.from = pos.x
            animY.from = pos.y
            animX.to = 0
            animY.to = 0
            animX.restart()
            animY.restart()
        }

        NumberAnimation {
            id: animX
            target: dragDummy
            duration: 250
            easing.type: Easing.OutQuad
            property: "x"
            running: false
            onFinished: {
                dragDummy.source = ""
                dragDummy.opacity = 0
            }
        }

        NumberAnimation {
            id: animY
            target: dragDummy
            duration: 250
            easing.type: Easing.OutQuad
            property: "y"
            running: false
            onFinished: {
                dragDummy.source = ""
                dragDummy.opacity = 0
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: dragDummy
            drag.target: dragDummy
            drag.filterChildren: true
            drag.onActiveChanged: {
                if (!mouseArea.drag.active) {
                    dragDummy.reparent(tile)
                    dragDummy.parent = tile
                    let action = tile.Drag.drop()
                } else {
                    dragDummy.opacity = 0.8
                    dragDummy.parent = ApplicationWindow.contentItem
                    tile.grabToImage(function (result) {
                        dragDummy.source = result.url
                    })
                }
            }
        }
    }
}
