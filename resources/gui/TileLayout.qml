import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12

Grid {

    id: grid

    readonly property real atomicWidth: 48
    readonly property real atomicHeight: 48

    readonly property int maxIndex: toIndex(rows - 1, columns - 1)

    columnSpacing: 4
    rowSpacing: 4

    rows: 6
    columns: 6

    add: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1.0
            duration: 400
            alwaysRunToEnd: true
        }
        NumberAnimation {
            property: "scale"
            from: 0
            to: 1.0
            duration: 400
            alwaysRunToEnd: true
        }
    }

    move: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 1000
        }
    }

    property int lastStackIndex: -1

    function dragEntered(row, column, drag) {

        if (drag.source && drag.source.dragActive) {

            var tile = drag.source
            d.dragActive = Qt.binding(function () {
                return tile.dragActive
            })
        }

        console.log("drag entered in row " + row + " column " + column + " source " + drag.source)

        lastStackIndex = d.index()
        console.log("saved undo stack index " + lastStackIndex)

        findSpot(drag.source, toIndex(row, column))

        drag.source.width = grid.atomicWidth * drag.source.columnSpan
                + (drag.source.columnSpan - 1) * grid.columnSpacing
        drag.source.height = grid.atomicHeight * drag.source.rowSpan
                + (drag.source.rowSpan - 1) * grid.rowSpacing
    }

    function dragExited(row, column, drag) {

        console.log("drag exited in row " + row + " column " + column)


        /*
        for (var i = column; i < column + columnSpan && hasSpace; i++) {
            for (var ii = row; ii < row + rowSpan && hasSpace; ii++) {
                let tileHolder = tileHolderAt(toIndex(ii, i))
                if (!tileHolder || (tileHolder && tileHolder.tile
                                    && tileHolder.tile !== tile)) {
                    hasSpace = false
                }
            }
        }*/
    }

    function dragFinished(row, column, drag) {

        console.log("drag finished in row " + row + " column " + column + " spanning rows "
                    + drag.source.rowSpan + ", columns " + drag.source.columnSpan)

        console.log("adding to tile holder in row" + drag.tileHolder.row
                    + " column " + drag.tileHolder.column)

        makePermanent()
        if (canMoveTile(drag.source, toIndex(row, column))) {
            moveTile(drag.source, toIndex(row, column), [], true)
        }
    }

    function addTile(tile, targetIndex) {

        let tileHolderTarget = tileHolderAt(targetIndex)
        if (tileHolderTarget) {
            if (!tileHolderTarget.tile) {
                console.log("Adding tile to " + targetIndex)
                tile.reparent(tileHolderTarget)
                tileHolderTarget.tile = tile
            }
        }
    }

    function canMoveTile(tile, targetIndex, ignoreTiles) {

        if (tile) {
            let row = toPosition(targetIndex).row
            let column = toPosition(targetIndex).column
            let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                   grid.rows)
            let columnSpan = Math.min(Math.max(Math.round(tile.columnSpan), 1),
                                      grid.columns)
            let ignore = [tile]
            if (Array.isArray(ignoreTiles)) {
                ignore = ignore.concat(ignoreTiles)
            }
            let hasSpace = true
            for (var i = column; i < column + columnSpan && hasSpace; i++) {
                for (var ii = row; ii < row + rowSpan && hasSpace; ii++) {
                    let tileHolder = tileHolderAt(toIndex(ii, i))
                    if (!tileHolder || (tileHolder && tileHolder.tile
                                        && !ignore.includes(tileHolder.tile))) {
                        hasSpace = false
                    }
                }
            }
            return hasSpace
        }
        return false
    }

    function resetTiles() {
        console.warn("----- reset")
        for (let key in tmpMovedTile) {
            let tileHolder = tmpMovedTile[key]
            if (tileHolder && tileHolder.tile) {
                tileHolder.tile.reparent(tileHolder)
            }
        }
        tmpMovedTile.length = 0
    }

    function makePermanent() {
        console.warn("----- make permanent")
        for (let key in tmpMovedTile) {
            let tileHolder = tmpMovedTile[key]
            if (tileHolder && tileHolder.tile) {
                moveTile(tileHolder.tile,
                         toIndex(tileHolder.tile.row,
                                 tileHolder.tile.column), [], true)
            }
        }
        tmpMovedTile.length = 0
    }

    // If permanent == false no canMoveTile checks are performed
    function moveTile(tile, targetIndex, ignoreTiles, permanent) {

        let tileHolderTarget = tileHolderAt(targetIndex)
        console.warn("--------------- move tile " + tile
                     + " -- holder target -- " + tileHolderTarget)
        if (tile && tileHolderTarget) {

            let row = toPosition(targetIndex).row
            let column = toPosition(targetIndex).column
            let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                   grid.rows)
            let columnSpan = Math.min(Math.max(Math.round(tile.columnSpan), 1),
                                      grid.columns)

            let rowOrigin = tile.row
            let columnOrigin = tile.column

            if (permanent) {

                // rowOrigin and columnOrigin may only point to the visual position (if the tile was moved temporarly). We need the owning parent
                for (let key in tmpMovedTile) {
                    let owningTileHolder = tmpMovedTile[key]
                    if (owningTileHolder) {
                        let potentialTile = owningTileHolder.tile
                        if (potentialTile === tile) {
                            rowOrigin = owningTileHolder.row
                            columnOrigin = owningTileHolder.column
                            break
                        }
                    }
                }

                if (rowOrigin >= 0 && columnOrigin >= 0) {
                    for (var i = columnOrigin; i < columnOrigin + columnSpan; i++) {
                        for (var ii = rowOrigin; ii < rowOrigin + rowSpan; ii++) {
                            let tileHolder = tileHolderAt(toIndex(ii, i))
                            if (tileHolder && tileHolder.tile === tile) {
                                tileHolder.tile = null
                                tileHolder.tileIndex = 0
                            }
                        }
                    }
                }
            }

            tile.reparent(tileHolderTarget)

            if (permanent) {
                let counter = 0
                for (var i = row; i < row + rowSpan; i++) {
                    for (var ii = column; ii < column + columnSpan; ii++) {
                        let tileHolder = tileHolderAt(toIndex(i, ii))
                        if (tileHolder) {
                            tileHolder.tile = tile
                            tileHolder.tileIndex = counter++
                        }
                    }
                }
            }
        }
    }

    function tileHolderAt(index) {

        return repeater.itemAt(index)
    }

    function tileAt(index) {

        if (tileHolderAt(index))
            return tileHolderAt(index).tile
        return undefined
    }

    readonly property var tmpMovedTile: []

    //! Find a free spot for the tile at index
    function findSpot(tile, index) {
        let tileHolder = tileHolderAt(index)

        if (tile && tileHolder && tileHolder.tile && tile !== tileHolder.tile) {

            let tileMove = tileHolder.tile
            let owningTileHolderMove = tileHolderAt(
                    toIndex(
                        tileHolder.row - Math.floor(
                            tileHolder.tileIndex / tileMove.columnSpan),
                        tileHolder.column - tileHolder.tileIndex % tileMove.columnSpan))
            let tileMoveRow = tileHolder.row - Math.floor(
                    tileHolder.tileIndex / tileMove.columnSpan)
            let tileMoveColumn = tileHolder.column - tileHolder.tileIndex % tileMove.columnSpan

            let tileRowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                       grid.rows)
            let tileColumnSpan = Math.min(Math.max(Math.round(tile.columnSpan),
                                                   1), grid.columns)
            let tileMoveRowSpan = Math.min(Math.max(Math.round(
                                                        tileMove.rowSpan), 1),
                                           grid.rows)
            let tileMoveColumnSpan = Math.min(
                    Math.max(Math.round(tileMove.columnSpan), 1), grid.columns)

            function intersectRect(index, targetIndex) {

                let targetTop = toPosition(targetIndex).row
                let targetLeft = toPosition(targetIndex).column
                let targetBottom = toPosition(
                        targetIndex).row + tileMoveRowSpan - 1
                let targetRight = toPosition(
                        targetIndex).column + tileMoveColumnSpan - 1
                let srcTop = toPosition(index).row
                let srcLeft = toPosition(index).column
                let srcBottom = toPosition(index).row + tileRowSpan - 1
                let srcRight = toPosition(index).column + tileColumnSpan - 1

                var leftX = Math.max(srcLeft, targetLeft)
                var rightX = Math.min(srcRight, targetRight)
                var topY = Math.max(srcTop, targetTop)
                var bottomY = Math.min(srcBottom, targetBottom)

                return Qt.rect(leftX, topY, rightX - leftX, bottomY - bottomY)
            }

            function intersects(index, targetIndex) {

                let targetTop = toPosition(targetIndex).row
                let targetLeft = toPosition(targetIndex).column
                let targetBottom = toPosition(
                        targetIndex).row + tileMoveRowSpan - 1
                let targetRight = toPosition(
                        targetIndex).column + tileMoveColumnSpan - 1
                let srcTop = toPosition(index).row
                let srcLeft = toPosition(index).column
                let srcBottom = toPosition(index).row + tileRowSpan - 1
                let srcRight = toPosition(index).column + tileColumnSpan - 1
                let ret = srcLeft <= targetRight && srcRight >= targetLeft
                    && srcTop <= targetBottom && srcBottom >= targetTop
                console.warn(
                            "-------- intersects " + ret + " src (" + srcLeft
                            + "," + srcTop + ")(" + srcRight + "," + srcBottom
                            + ")" + " target (" + targetLeft + "," + targetTop + ")("
                            + targetRight + "," + targetBottom + ")" + " -- " + intersectRect(
                                index, targetIndex))
                return ret
            }

            let moveDownIndex = toIndex(tileMoveRow + tileRowSpan,
                                        tileMoveColumn)
            let moveUpIndex = toIndex(tileMoveRow - tileRowSpan, tileMoveColumn)
            let moveLeftIndex = toIndex(tileMoveRow,
                                        tileMoveColumn - tileColumnSpan)
            let moveRightIndex = toIndex(tileMoveRow,
                                         tileMoveColumn + tileColumnSpan)

            if (canMoveTile(tileMove, moveRightIndex,
                            [tile]) && !intersects(index, moveRightIndex)) {
                //Move right
                grid.tmpMovedTile.push(owningTileHolderMove)
                moveTile(tileMove, moveRightIndex, [], false)
                console.warn("-------- move right")
            } else if (canMoveTile(tileMove, moveLeftIndex,
                                   [tile]) && !intersects(index,
                                                          moveLeftIndex)) {
                //Move left
                grid.tmpMovedTile.push(owningTileHolderMove)
                moveTile(tileMove, moveLeftIndex, [], false)
                console.warn("-------- move left")
            } else if (canMoveTile(tileMove, moveDownIndex,
                                   [tile]) && !intersects(index,
                                                          moveDownIndex)) {
                // Move down
                grid.tmpMovedTile.push(owningTileHolderMove)
                moveTile(tileMove, moveDownIndex, [], false)
                console.warn("-------- move down")
            } else if (canMoveTile(tileMove, moveUpIndex,
                                   [tile]) && !intersects(index, moveUpIndex)) {
                // Move up
                grid.tmpMovedTile.push(owningTileHolderMove)
                moveTile(tileMove, moveUpIndex, [], false)
                console.warn("-------- move up")
            } else {
                console.warn("-------- nowhere to move")
            }
        } else {

            resetTiles()
        }
    }

    function toIndex(row, column) {
        if (row < grid.rows && column < grid.columns) {
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

    QtObject {
        id: d

        function index() {

            return d.undoRedoIndex
        }

        function revertBack(index) {

            if (index >= 0 && index < moveHistory.length) {
                for (var i = moveHistory.length - 1; i >= index; i--) {
                    d.popUndoRedoStack()
                }
            }
        }

        function pushUndoRedoStack(redo, undo) {

            console.log("undo/redo command pushed to index " + (d.undoRedoIndex))

            d.moveHistory.push({
                                   "index": d.undoRedoIndex,
                                   "redo": redo,
                                   "undo": undo
                               })
            redo()

            d.undoRedoIndex++
        }

        function popUndoRedoStack() {

            if (d.undoRedoIndex >= 0) {

                console.log("undo/redo command poped " + (d.undoRedoIndex - 1))

                let op = d.moveHistory.pop()
                if (op)
                    op.undo()
                d.undoRedoIndex--
            }
        }

        property bool dragActive: false
        property int undoRedoIndex: 0
        property var moveHistory: []
    }

    Repeater {

        id: repeater

        model: ListModel {

            id: tileModel

            property int columns: grid.columns
            property int rows: grid.rows

            property int currentRows: 0
            property int currentColumns: 0

            onColumnsChanged: {

                if (tileModel.columns < 0)
                    return

                let currentNumColumns = tileModel.currentColumns

                tileModel.currentColumns = tileModel.columns

                console.info("Number of columns changed from "
                             + currentNumColumns + " to " + tileModel.columns
                             + " (current number of rows " + tileModel.currentRows + ")")

                if (currentNumColumns < tileModel.columns) {
                    // We have to add columns
                    let numColumnsToAdd = tileModel.columns - currentNumColumns
                    console.info("Adding " + numColumnsToAdd + " columns to model")
                    for (var i = currentNumColumns; i < tileModel.columns; i++) {
                        for (var ii = 0; ii < tileModel.currentRows; ii++) {
                            let indexToAdd = ii * tileModel.columns + i
                            console.info("Adding intex " + indexToAdd + " to model (row: "
                                         + ii + ", column: " + i + ")")
                            tileModel.insert(indexToAdd, {})
                        }
                    }
                } else if (currentNumColumns > tileModel.columns) {
                    // we have to remove columns
                    let numColumnsToRemove = currentNumColumns - tileModel.columns
                    console.info("Removing " + numColumnsToRemove + " columns from model")
                    for (var i = currentNumColumns - 1; i >= tileModel.columns; i--) {
                        for (var ii = tileModel.currentRows - 1; ii >= 0; ii--) {
                            let indexToRemove = ii * currentNumColumns + i
                            console.info("Removing intex " + indexToRemove
                                         + " from model (row: " + ii + ", column: " + i + ")")
                            tileModel.remove(indexToRemove)
                        }
                    }
                }
            }

            onRowsChanged: {

                if (tileModel.rows < 0)
                    return

                let currentNumRows = tileModel.currentRows

                tileModel.currentRows = tileModel.rows

                console.info("Number of rows changed from " + currentNumRows + " to "
                             + tileModel.rows + " (current number of columns "
                             + tileModel.currentColumns + ")")

                if (currentNumRows < tileModel.rows) {
                    // we have to add rows
                    let numRowsToAdd = tileModel.rows - currentNumRows
                    console.info("Adding " + numRowsToAdd + " rows to model")
                    for (var i = currentNumRows; i < tileModel.rows; i++) {
                        for (var ii = 0; ii < tileModel.currentColumns; ii++) {
                            let intexToAdd = i * tileModel.currentColumns + ii
                            console.info("Adding intex " + intexToAdd + " to model (row: "
                                         + i + ", column: " + ii + ")")
                            tileModel.insert(intexToAdd, {})
                        }
                    }
                } else if (currentNumRows > tileModel.rows) {
                    // we have to remove rows
                    let numRowsToRemove = currentNumRows - tileModel.rows
                    console.info("Removing " + numRowsToRemove + "rows from model")
                    for (var i = currentNumRows - 1; i >= tileModel.rows; i--) {
                        for (var ii = tileModel.currentColumns - 1; ii >= 0; ii--) {
                            let indexToRemove = i * tileModel.currentColumns + ii
                            console.info("Removing intex " + indexToRemove
                                         + " from model (row: " + i + ", column: " + ii + ")")
                            tileModel.remove(indexToRemove)
                        }
                    }
                }
            }
        }

        delegate: Rectangle {

            id: tileHolder
            border.width: 1
            border.color: "black"
            color: "transparent"

            property Tile tile: null
            property int tileIndex: 0
            property bool ownedTile: false

            readonly property int modelIndex: index
            property int row: grid.toPosition(index).row
            property int column: grid.toPosition(index).column

            property int rowSpan: 1
            property int columnSpan: 1

            objectName: "TileHolder(" + row + "," + column + ")"

            implicitHeight: grid.atomicHeight
            implicitWidth: grid.atomicWidth

            property var highlightItem: null

            property bool containsDrag: drop.containsDrag

            onContainsDragChanged: {
                let highlightItemWidth = grid.atomicWidth
                let highlightItemHeight = grid.atomicHeight
                let tile = drop.drag.source
                if (tile) {
                    let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan),
                                                    1), grid.rows)
                    let columnSpan = Math.min(Math.max(Math.round(
                                                           tile.columnSpan),
                                                       1), grid.columns)
                    highlightItemWidth = columnSpan * grid.atomicWidth
                            + (columnSpan - 1) * grid.columnSpacing
                    highlightItemHeight = rowSpan * grid.atomicHeight
                            + (rowSpan - 1) * grid.rowSpacing
                }

                if (containsDrag) {
                    highlightItem = highlightComponent.createObject(tileHolder,
                                                                    {
                                                                        "width": highlightItemWidth,
                                                                        "height": highlightItemHeight,
                                                                        "z": -1
                                                                    })
                } else {
                    if (highlightItem)
                        highlightItem.destroy()
                }
            }

            z: (ownedTile ? 1 : 0) + (tile
                                      && tile.dragActive ? 1 : 0) //+ (containsDrag ? 1 : 0)
            onTileChanged: {
                if (tile) {
                    var owned = false
                    for (var child in tileHolder.children) {
                        if (tileHolder.children[child] === tileHolder.tile) {
                            owned = true
                            //tile.scale = Qt.binding(function () {
                            //    return d.dragActive
                            //            && !this.dragActive ? 0.9 : 1
                            //})
                            break
                        }
                    }
                    if (owned !== tileHolder.ownedTile) {
                        tileHolder.ownedTile = owned
                    }
                    console.log("Gained tile at index " + index
                                + (tileHolder.ownedTile ? " owning it" : ""))
                    tileHolder.rowSpan = Qt.binding(function () {
                        return tile.dragActive ? 1 : tile.rowSpan
                    })
                    tileHolder.columnSpan = Qt.binding(function () {
                        return tile.dragActive ? 1 : tile.columnSpan
                    })
                } else {
                    tileHolder.ownedTile = false
                    console.log("Lost tile at index " + index)
                    tileHolder.rowSpan = 1
                    tileHolder.columnSpan = 1
                }
            }

            Label {
                text: index + " " + z
                anchors.top: parent.top
                anchors.left: parent.left
            }

            Label {
                text: tileHolder.tile ? (tileHolder.ownedTile ? "To" + tileIndex : "T"
                                                                + tileIndex) : ""
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }

            Behavior on implicitWidth {
                enabled: true
                NumberAnimation {
                    duration: 150
                    easing.type: Easing.InCubic
                }
            }

            Behavior on implicitHeight {
                enabled: true
                NumberAnimation {
                    duration: 150
                    easing.type: Easing.InCubic
                }
            }

            Behavior on scale {
                enabled: true
                NumberAnimation {
                    duration: 150
                    easing.type: Easing.InCubic
                }
            }

            //scale: d.dragActive ? 0.9 : 1
            DropArea {
                id: drop
                scale: 1 / parent.scale
                anchors.fill: parent
                anchors.leftMargin: -grid.columnSpacing / 2
                anchors.rightMargin: -grid.columnSpacing / 2
                anchors.topMargin: -grid.rowSpacing / 2
                anchors.bottomMargin: -grid.rowSpacing / 2

                onEntered: {
                    drag.tileHolder = tileHolder
                    if (false && !grid.canMoveTile(drag.source, grid.toIndex(
                                                       tileHolder.row,
                                                       tileHolder.column))) {
                        console.warn("drag rejected")
                        drag.accepted = false
                    } else
                        grid.dragEntered(tileHolder.row,
                                         tileHolder.column, drag)
                }

                onExited: {
                    drag.tileHolder = tileHolder
                    grid.dragExited(tileHolder.row, tileHolder.column, drag)
                }

                onDropped: {
                    drop.accept(Qt.MoveAction)
                    drop.tileHolder = tileHolder
                    grid.dragFinished(tileHolder.row, tileHolder.column, drop)
                }
            }
        }
    }

    Component {
        id: highlightComponent

        Rectangle {

            color: "green"
            opacity: 0.5
        }
    }
}
