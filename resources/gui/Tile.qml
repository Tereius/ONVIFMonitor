import QtQuick 2.0

Rectangle {

    id: tile

    readonly property int row: parent && parent.row != null ? parent.row : -1
    readonly property int column: parent
                                  && parent.column != null ? parent.column : -1
    readonly property int index: parent
                                 && parent.modelIndex != null ? parent.modelIndex : -1

    property int rowSpan: 1
    property int columnSpan: 1

    property bool dragActive: mouseArea.drag.active

    implicitHeight: 48 * rowSpan
    implicitWidth: 48 * columnSpan

    //anchors.top: parent ? parent.top : undefined
    //anchors.left: parent ? parent.left : undefined
    color: "red"
    objectName: "item-(" + row + ", " + column + ")"

    Drag.keys: [objectName]
    Drag.active: mouseArea.drag.active
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

    onStateChanged: {
        console.warn("state changed: " + state)
    }

    states: [
        State {
            when: !mouseArea.drag.active
            name: "dragging"


            /*
            ParentChange {
                id: parentChange
                target: tile
            }*/
            PropertyChanges {
                target: tile
                x: 0
                y: 0
            }
        }
    ]

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


    /*
    Behavior on width {
        enabled: tile.Drag.active
        NumberAnimation {
            duration: 150
            easing.type: Easing.InCubic
        }
    }

    Behavior on height {
        enabled: tile.Drag.active
        NumberAnimation {
            duration: 150
            easing.type: Easing.InCubic
        }
    }
*/
    Text {
        anchors.centerIn: parent
        text: tile.row + "," + tile.column + " " + tile.index
        color: "white"
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        drag.target: tile

        onReleased: {
            var dropAction = tile.Drag.drop()
            console.info("Drop accepted " + tile.Drag.target + " action "
                         + (dropAction != Qt.IgnoreAction ? "accepted" : "rejected"))
            //if (dropAction !== Qt.IgnoreAction && tile.Drag.target)
            //tile.parent = tile.Drag.target
        } //tile.parent = tile.Drag.target !== null ? tile.Drag.target : tile.parent
    }
}
