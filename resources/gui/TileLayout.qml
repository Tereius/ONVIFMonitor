import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12

Control {

    id: control
    focusPolicy: Qt.StrongFocus
    focus: true

    Shortcut {
        sequence: StandardKey.Undo
        onActivated: d.model.undo()
    }

    Shortcut {
        sequence: StandardKey.Redo
        onActivated: d.model.redo()
    }


    /**
     * Create a user
     * @param type:string firstname User firstname
     * @param type:string lastname User lastname
     * @param type:int User age
     * @return type:User The User object
     */
    property Transition add: Transition {
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

    property Transition move: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 1000
        }
    }

    property Component highlightDelegate: Rectangle {

        color: "red"
        opacity: 0.5
    }

    contentItem: Grid {

        id: grid

        readonly property real atomicWidth: 48
        readonly property real atomicHeight: 48

        readonly property int maxIndex: toIndex(rows - 1, columns - 1)

        //columnSpacing: 4
        //rowSpacing: 4
        rows: 6
        columns: 6

        add: control.add

        move: control.move

        function addTile(tile, targetIndex) {

            tile.visible = true
            return moveTile(tile, targetIndex)
        }

        function removeTile(tile) {

            if (tile) {

                tile.visible = false
                let rowOrigin = tile.row
                let columnOrigin = tile.column
                let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                       grid.rows)
                let columnSpan = Math.min(Math.max(Math.round(tile.columnSpan),
                                                   1), grid.columns)
                let isNewTile = rowOrigin < 0 || columnOrigin < 0

                // Detatch the tile from its old tileHolder
                if (!isNewTile) {
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
        }

        function canMoveTile(tile, targetIndex, ignoreTiles) {

            return d.canMoveTile(grid, tile, targetIndex, ignoreTiles)
        }

        function moveTile(tile, targetIndex) {

            let tileHolderTarget = tileHolderAt(targetIndex)
            console.warn("--------------- move tile " + tile
                         + " -- holder target -- " + tileHolderTarget)
            if (tile && tileHolderTarget) {

                let rowTarget = toPosition(targetIndex).row
                let columnTarget = toPosition(targetIndex).column

                let rowOrigin = tile.row
                let columnOrigin = tile.column
                let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                       grid.rows)
                let columnSpan = Math.min(Math.max(Math.round(tile.columnSpan),
                                                   1), grid.columns)
                let isNewTile = rowOrigin < 0 || columnOrigin < 0

                // Detatch the tile from its old tileHolder
                if (!isNewTile) {
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

                tile.reparent(tileHolderTarget)

                let counter = 0
                // Attach the tile to its new tileHolder
                for (var i = rowTarget; i < rowTarget + rowSpan; i++) {
                    for (var ii = columnTarget; ii < columnTarget + columnSpan; ii++) {
                        let tileHolder = tileHolderAt(toIndex(i, ii))
                        if (tileHolder) {
                            tileHolder.tile = tile
                            tileHolder.tileIndex = counter++
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
            property var dragTile: null
            property var dragModelTile: null
            property bool dragActive: false

            onDragActiveChanged: {
                grid.dragCanceled()
            }

            property var visualTiles: null

            Component.onCompleted: {
                d.visualTiles = {}
            }

            property TileLayoutLogic model: TileLayoutLogic {
                data: TileLayoutData {
                    rows: grid.rows
                    columns: grid.columns
                }

                onTileAdded: function (tile, index) {

                    let visualTile = d.visualTiles[tile.id]
                    if (visualTile) {
                        grid.addTile(visualTile, index)
                    } else {
                        console.error(
                                    "Could not find a visual tile with id " + tile.id)
                    }
                }

                onTileMoved: function (fromIndex, toIndex) {

                    grid.moveTile(grid.tileAt(fromIndex), toIndex)
                }

                onTileRemoved: function (tile) {

                    let visualTile = d.visualTiles[tile.id]
                    if (visualTile) {
                        grid.removeTile(visualTile)
                    } else {
                        console.error(
                                    "Could not find a visual tile with id " + tile.id)
                    }
                }
            }

            function canMoveTile(model, tile, targetIndex, ignoreTiles) {

                if (model && tile) {
                    let row = model.toPosition(targetIndex).row
                    let column = model.toPosition(targetIndex).column
                    let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan),
                                                    1), model.rows)
                    let columnSpan = Math.min(Math.max(Math.round(
                                                           tile.columnSpan),
                                                       1), model.columns)
                    let ignore = [tile]
                    if (Array.isArray(ignoreTiles)) {
                        ignore = ignore.concat(ignoreTiles)
                    }
                    let hasSpace = true
                    for (var i = column; i < column + columnSpan
                         && hasSpace; i++) {
                        for (var ii = row; ii < row + rowSpan
                             && hasSpace; ii++) {
                            let tileHolder = model.tileHolderAt(toIndex(ii, i))
                            if (!tileHolder || (tileHolder && tileHolder.tile
                                                && !ignore.includes(
                                                    tileHolder.tile))) {
                                hasSpace = false
                            }
                        }
                    }
                    return hasSpace
                }
                return false
            }
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

                    console.info(
                                "Number of columns changed from "
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

                    console.info("Number of rows changed from " + currentNumRows
                                 + " to " + tileModel.rows + " (current number of columns "
                                 + tileModel.currentColumns + ")")

                    if (currentNumRows < tileModel.rows) {
                        // we have to add rows
                        let numRowsToAdd = tileModel.rows - currentNumRows
                        console.info(
                                    "Adding " + numRowsToAdd + " rows to model")
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

                // public
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
                    let tileModel = null

                    if (tile) {
                        tileModel = d.model.tileAt(d.model.toIndex(tile.row,
                                                                   tile.column))
                        if (!tileModel)
                            tileModel = tile
                        let rowSpan = Math.min(Math.max(Math.round(
                                                            tile.rowSpan), 1),
                                               grid.rows)
                        let columnSpan = Math.min(
                                Math.max(Math.round(tile.columnSpan), 1),
                                grid.columns)
                        highlightItemWidth = columnSpan * grid.atomicWidth
                                + (columnSpan - 1) * grid.columnSpacing
                        highlightItemHeight = rowSpan * grid.atomicHeight
                                + (rowSpan - 1) * grid.rowSpacing
                    }

                    if (containsDrag && d.model.canMoveTile(tileModel,
                                                            d.model.toIndex(
                                                                row, column))) {
                        if (control.highlightDelegate) {
                            highlightItem = control.highlightDelegate.createObject(
                                        tileHolder, {
                                            "width": highlightItemWidth,
                                            "height": highlightItemHeight,
                                            "z": -1
                                        })
                        }
                    } else {
                        if (highlightItem)
                            highlightItem.destroy()
                    }
                }

                z: (ownedTile ? 1 : 0) + (tile && tile.dragActive ? 1 : 0)
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

                DropArea {
                    id: drop
                    scale: 1 / parent.scale
                    keys: ["tile"]
                    anchors.fill: parent
                    anchors.leftMargin: -grid.columnSpacing / 2
                    anchors.rightMargin: -grid.columnSpacing / 2
                    anchors.topMargin: -grid.rowSpacing / 2
                    anchors.bottomMargin: -grid.rowSpacing / 2

                    function dragCanceled() {}

                    onEntered: drag => {

                                   if (drag.source
                                       && drag.source instanceof Tile) {
                                       var tile = drag.source

                                       let modelTile = d.model.tileAt(
                                           d.model.toIndex(tile.row,
                                                           tile.column))

                                       if (tile.row < 0 && tile.column < 0) {

                                           // A new tile
                                           console.debug(
                                               "A new tile entered " + tileHolder.objectName)
                                       } else {
                                           console.debug(
                                               "An existing tile entered " + tileHolder.objectName)
                                       }

                                       if (!modelTile) {
                                           // A new tile
                                           modelTile = d.model._private.newTile(
                                               -1, -1, drag.source.rowSpan,
                                               drag.source.columnSpan)
                                       }

                                       console.log(
                                           "drag entered in row " + row + " column "
                                           + column + " source " + drag.source)

                                       d.model.proposeMoveTile(modelTile,
                                                               d.model.toIndex(
                                                                   row, column))
                                   }
                               }

                    onDropped: drop => {

                                   if (drop.source
                                       && drop.source instanceof Tile) {

                                       var tile = drop.source
                                       let modelTile = d.model.tileAt(
                                           d.model.toIndex(tile.row,
                                                           tile.column))

                                       if (tile.row < 0 && tile.column < 0) {

                                           // A new tile
                                           console.debug(
                                               "A new tile dropped " + tileHolder.objectName)
                                       } else {
                                           console.debug(
                                               "An existing tile dropped " + tileHolder.objectName)
                                       }

                                       if (!modelTile) {
                                           // A new tile
                                           modelTile = d.model._private.newTile(
                                               -1, -1, tile.rowSpan,
                                               tile.columnSpan)

                                           console.warn(
                                               "------" + tile + " " + d.visualTiles)
                                           d.visualTiles[modelTile.id] = tile
                                       }

                                       drop.accept(Qt.MoveAction)

                                       console.log(
                                           "drop finished in row " + row + " column "
                                           + column + " spanning rows " + tile.rowSpan
                                           + ", columns " + tile.columnSpan)

                                       console.log(
                                           "adding to tile holder in row " + row
                                           + " column " + column)

                                       d.model.finishProposeMoveTile(
                                           modelTile,
                                           d.model.toIndex(row, column))
                                   }
                               }
                }
            }
        }
    }
}
