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

    anchors.top: parent ? parent.top : undefined
    anchors.left: parent ? parent.left : undefined

    color: "red"
    objectName: "item-(" + row + ", " + column + ")"

    Drag.keys: [objectName]
    Drag.active: mouseArea.drag.active
    Drag.hotSpot.x: width / columnSpan / 2
    Drag.hotSpot.y: height / rowSpan / 2
    states: State {
        when: mouseArea.drag.active
        //ParentChange {
        //    target: tile
        //    parent: root
        //}
        AnchorChanges {
            target: tile
            anchors.top: undefined
            anchors.left: undefined
        }
    }

    transitions: Transition {
        ParentAnimation {
            NumberAnimation {
                properties: "x,y"
                duration: 1000
            }
        }
    }

    Behavior on width {
        enabled: tile.Drag.active
        NumberAnimation {
            duration: 150
            easing: Easing.InCubic
        }
    }

    Behavior on height {
        enabled: tile.Drag.active
        NumberAnimation {
            duration: 150
            easing: Easing.InCubic
        }
    }

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
            console.info("Drop accepted " + tile.Drag.target)
            //if (dropAction !== Qt.IgnoreAction && tile.Drag.target)
            //tile.parent = tile.Drag.target
        } //tile.parent = tile.Drag.target !== null ? tile.Drag.target : tile.parent
    }
}
