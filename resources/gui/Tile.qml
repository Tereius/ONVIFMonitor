import QtQuick 2.0

Rectangle {

    id: tile

    readonly property int row: parent && parent.row != null ? parent.row : -1
    readonly property int column: parent
                                  && parent.column != null ? parent.column : -1
    readonly property int index: parent
                                 && parent.modelIndex != null ? parent.modelIndex : -1

    readonly property bool dragActive: mouseArea.drag.active

    property int rowSpan: 1
    property int columnSpan: 1

    implicitHeight: 48 * rowSpan
    implicitWidth: 48 * columnSpan

    //anchors.top: parent ? parent.top : undefined
    //anchors.left: parent ? parent.left : undefined
    color: "red"
    objectName: "item-(" + row + ", " + column + ")"

    Drag.keys: ["tile"]
    Drag.active: mouseArea.drag.active
    Drag.dragType: Drag.Internal
    Drag.hotSpot.x: width / columnSpan / 2
    Drag.hotSpot.y: height / rowSpan / 2

    function reparent(newParent) {
        if (newParent && newParent !== tile.parent) {
            let from = mapToItem(newParent, 0, 0)
            tile.parent = newParent
            animX.from = from.x
            animY.from = from.y
            animX.to = 0
            animY.to = 0
            animX.restart()
            animY.restart()
        }
    }

    NumberAnimation {
        id: animX
        target: tile
        duration: 250
        easing.type: Easing.OutQuad
        property: "x"
        running: false
    }

    NumberAnimation {
        id: animY
        target: tile
        duration: 250
        easing.type: Easing.OutQuad
        property: "y"
        running: false
    }

    Text {
        anchors.centerIn: parent
        text: tile.row + "," + tile.column + " " + tile.index
        color: "white"
    }

    MouseArea {
        id: mouseArea

        anchors.fill: tile
        drag.target: tile
        drag.filterChildren: true
        drag.onActiveChanged: {
            if (!mouseArea.drag.active) {
                tile.Drag.drop()
                tile.x = 0
                tile.y = 0
            }
        }
    }
}
