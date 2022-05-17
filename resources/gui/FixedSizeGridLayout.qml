import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import QtQuick.Layouts 1.12
import TilesGrid 1.0

Item {

    id: control

    property int rows: 0
    property int columns: 0
    property alias rowSpacing: grid.rowSpacing
    property alias columnSpacing: grid.columnSpacing
    // This is the width of the smallest tile (the tiles columnSpan equals 1)
    property real atomicWidth: 50
    // This is the height of the smallest tile (the tiles rowSpan equals 1)
    property real atomicHeight: 50

    property alias model: repeater.model
    property alias delegate: repeater.delegate

    readonly property real effectiveAtomicWidth: {

        let w = control.width // leve this here. leads to evaluation
        return w / grid.columns
    }

    readonly property real effectiveAtomicHeight: {

        let h = control.height // leve this here. leads to evaluation
        let item = repeater.itemAt(0)
        if (item) {
            return item.height
        }

        return 0
    }

    property alias count: repeater.count

    signal moveIndex(int fromIndex, int toIndex)

    onAtomicHeightChanged: {
        grid.atomicHeight = control.atomicHeight
    }

    onAtomicWidthChanged: {
        grid.atomicWidth = control.atomicWidth
    }

    //focusPolicy: Qt.StrongFocus
    focus: true

    function forceLayout() {
        grid.forceLayout()
    }

    Shortcut {
        sequence: StandardKey.Undo
        onActivated: d.model.undo()
    }

    Shortcut {
        sequence: StandardKey.Redo
        onActivated: d.model.redo()
    }

    property Transition add: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1.0
            duration: 400
        }
        NumberAnimation {
            property: "scale"
            from: 0
            to: 1.0
            duration: 400
        }
    }

    property Transition move: Transition {
        id: moveTransition
        NumberAnimation {
            property: "opacity"
            to: 1.0
            duration: 400
        }
        NumberAnimation {
            property: "scale"
            to: 1.0
            duration: 400
        }
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutCubic
            duration: moveTransition.ViewTransition.item === dropArea.currentDragTile ? 0 : 250
        }
    }

    property Component highlightDelegate: Rectangle {

        color: "red"
        opacity: 0.5
    }

    implicitHeight: grid.implicitHeight
    implicitWidth: grid.implicitWidth

    Item {

        implicitHeight: grid.implicitHeight
        implicitWidth: grid.implicitWidth

    Grid {

        id: grid

        property real atomicWidth: 50
        property real atomicHeight: 50

        columnSpacing: 0
        rowSpacing: 0
        rows: control.rows // no onRowsChanged signal emitted
        columns: control.columns

        property int rowsCount: grid.toPosition(repeater.count - 1).row + 1

        add: control.add
        move: control.move

        function tileAtPosition(x, y) {

            return grid.childAt(x, y)
        }

        function toIndex(row, column) {
            if (row < grid.rowsCount && column < grid.columns) {
                return row * grid.columns + column
            }
            return -1
        }

        function toPosition(index) {
            return {
                "row": Math.floor(index / grid.columns),
                "column": index % grid.columns
            }
        }

        Repeater {

            id: repeater

            delegate: control.delegate
        }
    }

    DropArea {
        id: dropArea
        keys: ["tile"]
        anchors.fill: parent

        property UndoStack stack: UndoStack {}

        property var currentDragTile: drag.source
                                      && drag.source instanceof Tile ? drag.source : null
        property int dragEnterStackIndex: 0
        property var currentFreeTileHolder: null

        function handleDrag(drag) {

            let tileHolder = null
            let canMove = false
            if (drag.source && drag.source instanceof Tile) {

                let mappedPoint = control.mapToItem(grid, drag.x, drag.y)
                let tile = drag.source

                let column = Math.min(
                        Math.floor(
                            mappedPoint.x / (grid.width / grid.columns)),
                        grid.columns - 1)

                let row = Math.min(
                        Math.floor(
                            mappedPoint.y / (grid.height / grid.rowsCount)),
                        grid.rowsCount - 1)

                let index = grid.toIndex(row, column)

                let tileMove = grid.tileAtPosition(mappedPoint.x, mappedPoint.y)
                if (index >= 0) {
                    control.moveIndex(tile.modelIndex, index)
                }
            } else {
                drag.accepted = false
            }
            return canMove
        }

        function cancelDrag(drag) {

            dropArea.currentFreeTileHolder = null
            dropArea.stack.setIndex(dropArea.dragEnterStackIndex)
            if (drag)
                drag.accepted = false
        }

        onPositionChanged: drag => {
                               handleDrag(drag)
                           }

        onEntered: drag => {
                       dropArea.dragEnterStackIndex = dropArea.stack.index
                       handleDrag(drag)
                   }

        onDropped: drop => {

                       dropArea.lastMove = ""

                       let canMove = handleDrag(drop)

                       let mappedPoint = control.mapToItem(grid, drag.x, drag.y)
                       let tileHolder = grid.tileHolderAtPosition(
                           mappedPoint.x, mappedPoint.y)
                       if (tileHolder && drop.source && canMove
                           && drop.source instanceof Tile) {
                           var tile = drop.source

                           drop.accept(Qt.MoveAction)

                           dropArea.stack.pushUndoRedoCommand(function () {
                               tile.reparent(dropArea.currentFreeTileHolder)
                           }, function () {
                               tile.reparent(tileMoveHolder)
                           })
                           dropArea.currentFreeTileHolder = null
                       } else {
                           drop.accept(Qt.IgnoreAction)
                           cancelDrag(drop)
                       }
                   }

        onExited: {
            dropArea.cancelDrag()
        }
    }
    }
}
