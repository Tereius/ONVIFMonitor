import QtQuick 2.0

QtObject {

    id: logic
    signal tileAdded(var tile, int index)
    signal tileMoved(int fromIndex, int toIndex)
    signal tileRemoved(var tile)
    signal collisionDetected(var model, var tile, var index, var intersections)

    property TileLayoutData data: TileLayoutData {}

    property alias rows: logic.data.rows
    property alias columns: logic.data.columns

    onTileAdded: (tile, index) => {
                     console.debug(
                         "emit tileAdded(" + tile + ", " + index + ")")
                     printLayout()
                 }

    onTileMoved: (fromIndex, toIndex) => {
                     console.debug(
                         "emit tileMoved(" + fromIndex + ", " + toIndex + ")")
                     printLayout()
                 }

    onTileRemoved: (tile, index) => {
                       console.debug(
                           "emit tileRemoved(" + tile + ", " + index + ")")
                       printLayout()
                   }

    onDataChanged: {

        for (let key in logic._private.data.tiles) {
            if (!logic.data.tiles.hasOwnProperty(key)) {
                // Tile was removed
                tileRemoved(logic._private.data.tiles[key])
            } else {

                // Check if tile was moved
            }
        }
    }

    onCollisionDetected: (model, tile, index, intersections) => {

                             console.debug(
                                 "emit collisionDetected(" + tile + ", " + index + ")")

                             for (let key in intersections) {
                                 // Some constants
                                 let tileRow = model.toPosition(index).row
                                 let tileColumn = model.toPosition(index).column
                                 let tileMove = intersections[key].tile
                                 let intersection = intersections[key].intersection
                                 let tileRowSpan = Math.min(
                                     Math.max(Math.round(tile.rowSpan), 1),
                                     model.rows)
                                 let tileColumnSpan = Math.min(
                                     Math.max(Math.round(tile.columnSpan), 1),
                                     model.columns)
                                 let tileMoveRowSpan = Math.min(
                                     Math.max(Math.round(tileMove.rowSpan), 1),
                                     model.rows)
                                 let tileMoveColumnSpan = Math.min(
                                     Math.max(Math.round(tileMove.columnSpan),
                                              1), model.columns)
                                 let maxWidthTile = tile.columnSpan
                                 > tileMove.columnSpan ? tile : tileMove
                                 let maxHeightTile = tile.rowSpan
                                 > tileMove.rowSpan ? tile : tileMove

                                 // Determine the affinity where the 'tileMove' wants to move
                                 let tileMoveLeftAffinity = intersection.x + intersection.width / 2
                                 > maxWidthTile.column + maxWidthTile.columnSpan / 2
                                 let tileMoveRightAffinity = intersection.x + intersection.width / 2
                                 < maxWidthTile.column + maxWidthTile.columnSpan / 2
                                 let tileMoveTopAffinity = intersection.y + intersection.height / 2
                                 > maxHeightTile.row + maxHeightTile.rowSpan / 2
                                 let tileMoveBottomAffinity = intersection.y + intersection.height
                                 / 2 < maxHeightTile.row + maxHeightTile.rowSpan / 2

                                 // The horizontal move affinity is inverted if the width of 'tile' is bigger than 'tileMove'
                                 if (maxWidthTile.equals(tile)) {
                                     let tmpTileMoveLeftAffinity = tileMoveLeftAffinity
                                     tileMoveLeftAffinity = tileMoveRightAffinity
                                     tileMoveRightAffinity = tmpTileMoveLeftAffinity
                                 }

                                 // The vertical move affinity is inverted if the height of 'tile' is bigger than 'tileMove'
                                 if (maxHeightTile.equals(tile)) {
                                     let tmpTileMoveTopAffinity = tileMoveTopAffinity
                                     tileMoveTopAffinity = tileMoveBottomAffinity
                                     tileMoveBottomAffinity = tmpTileMoveTopAffinity
                                 }

                                 if (!tileMoveLeftAffinity
                                     && !tileMoveRightAffinity
                                     && !tileMoveTopAffinity
                                     && !tileMoveBottomAffinity) {
                                     tileMoveLeftAffinity = tileMoveRightAffinity
                                     = tileMoveTopAffinity = tileMoveBottomAffinity = true
                                 }

                                 console.warn(
                                     "------- left " + tileMoveLeftAffinity + " rigth "
                                     + tileMoveRightAffinity + " top " + tileMoveTopAffinity
                                     + " bottom " + tileMoveBottomAffinity)

                                 // Calculate all intexes where 'tileMove' can potentially move (ignore collisions for now)
                                 let moveDownIndex = model.toIndex(
                                     tileRow + tileRowSpan, tileMove.column)
                                 let moveUpIndex = model.toIndex(
                                     tileRow - tileRowSpan, tileMove.column)
                                 let moveLeftIndex = model.toIndex(
                                     tileRow, tileColumn - tileColumnSpan)
                                 let moveRightIndex = model.toIndex(
                                     tileRow, tileColumn + tileColumnSpan)

                                 console.warn(
                                     "########### " + moveDownIndex + " " + moveUpIndex
                                     + " " + moveLeftIndex + " " + moveRightIndex
                                     + " --tileRow " + tileRow + " tileColumn " + tileColumn
                                     + " tileMove.column " + tileMove.column
                                     + " tileMove.row " + tileMove.row
                                     + " tileRowSpan " + tileRowSpan
                                     + " tileColumnSpan " + tileColumnSpan)

                                 if (tileMoveRightAffinity && model.canMoveTile(
                                         tileMove, moveRightIndex, [tile])) {
                                     model.moveTile(tileMove,
                                                    moveRightIndex, [])
                                     console.warn("-------- move right")
                                 } else if (tileMoveLeftAffinity
                                            && model.canMoveTile(tileMove,
                                                                 moveLeftIndex,
                                                                 [tile])) {
                                     model.moveTile(tileMove, moveLeftIndex, [])
                                     console.warn("-------- move left")
                                 } else if (tileMoveBottomAffinity
                                            && model.canMoveTile(tileMove,
                                                                 moveDownIndex,
                                                                 [tile])) {
                                     model.moveTile(tileMove, moveDownIndex, [])
                                     console.warn("-------- move down")
                                 } else if (tileMoveTopAffinity
                                            && model.canMoveTile(tileMove,
                                                                 moveUpIndex,
                                                                 [tile])) {
                                     model.moveTile(tileMove, moveUpIndex, [])
                                     console.warn("-------- move up")
                                 } else {
                                     console.warn("-------- nowhere to move")
                                 }
                             }
                         }

    function undo() {

        logic._private.undo()
    }

    function redo() {

        logic._private.redo()
    }

    function canMoveTile(tile, targetIndex, ignoreTiles) {

        if (tile) {
            let row = logic.toPosition(targetIndex).row
            let column = logic.toPosition(targetIndex).column
            let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                   logic.data.rows)
            let columnSpan = Math.min(Math.max(Math.round(tile.columnSpan), 1),
                                      logic.data.columns)
            let ignore = [tile]
            if (Array.isArray(ignoreTiles)) {
                ignore = ignore.concat(ignoreTiles)
            }
            let hasSpace = row + rowSpan <= logic.data.rows
                && column + columnSpan <= logic.data.columns
            for (var i = column; i < column + columnSpan && hasSpace; i++) {
                for (var ii = row; ii < row + rowSpan && hasSpace; ii++) {
                    let tileHolder = logic.tileHolderAt(logic.toIndex(ii, i))
                    if (!tileHolder || (tileHolder && tileHolder.tile
                                        && !ignore.some(
                                            iterTile => iterTile.equals(
                                                tileHolder.tile)))) {
                        hasSpace = false
                    }
                }
            }
            return hasSpace
        }
        return false
    }

    function addTile(tile, targetIndex, ignoreTiles) {

        let tileHolderTarget = logic.tileHolderAt(targetIndex)

        if (tile && tile.rowOrigin < 0 && tile.columnOrigin < 0
                && tileHolderTarget && logic.canMoveTile(tile, targetIndex,
                                                         ignoreTiles)) {
            let oldIndex = logic.toIndex(tile.row, tile.column)
            console.warn("add oldindex ----- " + tile)
            logic._private.pushUndoRedoCommand(function () {
                logic._moveTile(tile, targetIndex, ignoreTiles)
            }, function () {
                let movedTile = logic.tileAt(targetIndex)
                logic._moveTile(movedTile, oldIndex, ignoreTiles)
            })
        }
        moveTile(tile, targetIndex, ignoreTiles)
    }

    function moveTile(tile, targetIndex, ignoreTiles) {

        let tileHolderTarget = logic.tileHolderAt(targetIndex)

        if (tile && tileHolderTarget && logic.canMoveTile(tile, targetIndex,
                                                          ignoreTiles)) {
            let oldIndex = logic.toIndex(tile.row, tile.column)
            console.warn("oldindex ----- " + tile)
            logic._private.pushUndoRedoCommand(function () {
                logic._moveTile(tile, targetIndex, ignoreTiles)
            }, function () {
                let movedTile = logic.tileAt(targetIndex)
                logic._moveTile(movedTile, oldIndex, ignoreTiles)
            })
        }
    }

    function proposeMoveTile(tile, index) {

        if (logic._private.proposeMoveTile
                && logic._private.proposeMoveTile.equals(tile)) {
            logic._private.setIndex(logic._private.proposeMoveUndoStackIndex)
            console.warn("--------------- ############## back")
        } else {
            console.warn("--------------- ##############")
            logic._private.proposeMoveTile = tile
            logic._private.proposeMoveUndoStackIndex = logic._private.index()
        }

        let detectIntersection = function (tileHolder) {
            if (tile && tileHolder && tileHolder.tile && !tile.equals(
                        tileHolder.tile)) {

                let intersections = []
                let tileMove = tileHolder.tile

                let tileRowSpan = tile.rowSpan
                let tileColumnSpan = tile.columnSpan
                let tileMoveRowSpan = tileMove.rowSpan
                let tileMoveColumnSpan = tileMove.columnSpan

                let targetTop = tileMove.row
                let targetLeft = tileMove.column
                let targetBottom = tileMove.row + tileMoveRowSpan
                let targetRight = tileMove.column + tileMoveColumnSpan

                let srcTop = logic.toPosition(index).row
                let srcLeft = logic.toPosition(index).column
                let srcBottom = srcTop + tileRowSpan
                let srcRight = srcLeft + tileColumnSpan

                var leftX = Math.max(srcLeft, targetLeft)
                var rightX = Math.min(srcRight, targetRight)
                var topY = Math.max(srcTop, targetTop)
                var bottomY = Math.min(srcBottom, targetBottom)

                return Qt.rect(leftX, topY, rightX - leftX, bottomY - topY)
            }
        }

        let row = logic.toPosition(index).row
        let column = logic.toPosition(index).column
        let tileRowSpan = tile.rowSpan
        let tileColumnSpan = tile.columnSpan
        let intersections = []

        for (var i = column; i < column + tileColumnSpan; i++) {
            for (var ii = row; ii < row + tileRowSpan; ii++) {
                let tileHolder = logic.tileHolderAt(logic.toIndex(ii, i))
                let intersection = detectIntersection(tileHolder)
                if (intersection) {
                    let newIntersection = true
                    for (var key in intersections) {
                        if (intersections[key].tile.equals(tileHolder.tile)) {
                            newIntersection = false
                            break
                        }
                    }
                    if (newIntersection) {
                        let intersectionobj = {
                            "tile": tileHolder.tile,
                            "intersection": intersection
                        }
                        intersections.push(intersectionobj)
                        console.warn("------- intersection ",
                                     JSON.stringify(intersectionobj))
                    }
                }
            }
        }

        if (intersections.length > 0)
            logic.collisionDetected(d.model, tile, index, intersections)
    }

    function finishProposeMoveTile(tile, index) {

        if (logic._private.proposeMoveTile
                && logic._private.proposeMoveTile.equals(tile)) {
            moveTile(tile, index, [])
        }
        cancelProposeMoveTile()
    }

    function cancelProposeMoveTile() {

        logic._private.proposeMoveTile = null
        logic._private.proposeMoveUndoStackIndex = 0
    }

    // If the tile is a new one (row == -1 && column == -1) it will be added to the layout.
    // If 'targetIndex' is out of bounds the tile will be removed from the layout
    function _moveTile(tile, targetIndex, ignoreTiles) {

        let tileHolderTarget = logic.tileHolderAt(targetIndex)

        if (tile && (logic.canMoveTile(tile, targetIndex,
                                       ignoreTiles) || !tileHolderTarget)) {

            let rowTarget = logic.toPosition(targetIndex).row
            let columnTarget = logic.toPosition(targetIndex).column

            let rowOrigin = tile.row
            let columnOrigin = tile.column
            let rowSpan = Math.min(Math.max(Math.round(tile.rowSpan), 1),
                                   logic.data.rows)
            let columnSpan = Math.min(Math.max(Math.round(tile.columnSpan), 1),
                                      logic.data.columns)
            let isNewTile = rowOrigin < 0 || columnOrigin < 0
            let isRemovedTile = !tileHolderTarget
            if (!(isNewTile && isRemovedTile)) {

                // Detatch the tile from its old tileHolder
                if (!isNewTile) {
                    for (var i = columnOrigin; i < columnOrigin + columnSpan; i++) {
                        for (var ii = rowOrigin; ii < rowOrigin + rowSpan; ii++) {
                            delete logic.data.tiles[logic._private.toModelKey(
                                                        ii, i)]
                        }
                    }
                }

                let oldRow = tile.row
                let oldColumn = tile.column
                tile.row = rowTarget
                tile.column = columnTarget

                if (!isRemovedTile) {
                    // Attach the tile to its new tileHolder
                    for (var i = rowTarget; i < rowTarget + rowSpan; i++) {
                        for (var ii = columnTarget; ii < columnTarget + columnSpan; ii++) {
                            logic.data.tiles[logic._private.toModelKey(
                                                 i, ii)] = tile
                        }
                    }
                }

                if (isNewTile) {
                    console.log("Added " + tile + " to " + targetIndex)
                    logic.tileAdded(tile, targetIndex)
                } else if (isRemovedTile) {
                    console.log("Removed " + tile)
                    logic.tileRemoved(tile)
                } else {
                    console.log("Moved " + tile + " to " + targetIndex)
                    logic.tileMoved(logic.toIndex(oldRow, oldColumn),
                                    targetIndex)
                }
            } else {
                console.info("Tile is added and removed at the same time")
            }
        }
    }

    function tileHolderAt(index) {

        if (index < logic.data.rows * logic.data.columns && index >= 0) {
            let position = logic.toPosition(index)

            let tile = logic.data.tiles[logic._private.toModelKey(
                                            position.row, position.column)]
            return {
                "row": position.row,
                "column": position.column,
                "tile": tile,
                "tileIndex": tile ? (position.row - tile.row) * logic.data.columns
                                    + (position.column - tile.column) : 0
            }
        }
    }

    function tileAt(index) {

        if (logic.tileHolderAt(index))
            return logic.tileHolderAt(index).tile
    }

    function toIndex(row, column) {

        if (row >= 0 && row < logic.data.rows && column >= 0
                && column < logic.data.columns) {
            return row * logic.data.columns + column
        }
        return -1
    }

    function toPosition(index) {

        if (index >= 0 && index < logic.data.columns * logic.data.rows) {
            return {
                "row": Math.floor(index / logic.data.columns),
                "column": index % logic.data.columns
            }
        }
        return {
            "row": -1,
            "column": -1
        }
    }

    function printLayout() {

        for (var i = 0; i < logic.data.rows; i++) {
            let printLine = ""
            for (var ii = 0; ii < logic.data.columns; ii++) {
                let index = logic.toIndex(i, ii)
                let tileHolder = logic.tileHolderAt(index)
                if (tileHolder && tileHolder.tile) {
                    for (var iii = 0; iii < 2; iii++)
                        printLine += "█"
                    printLine += " "
                } else {
                    let indexString = String(index)
                    for (var iii = 0; iii < 2 - indexString.length; iii++)
                        printLine += "0"
                    printLine += indexString + " "
                }
            }
            console.info(printLine)
        }
    }

    readonly property QtObject _private: QtObject {

        property TileLayoutData data: logic.data

        property int proposeMoveUndoStackIndex: 0
        property var proposeMoveTile: null

        function pushUndoRedoCommand(redoFunctor, undoFunctor) {

            console.assert(typeof redoFunctor === "function",
                           "redoFunctor not a function")
            console.assert(typeof undoFunctor === "function",
                           "undoFunctor not a function")
            console.assert(
                        logic.data.undoStackIndex <= logic.data.undoStack.length,
                        "Current undoStack index (" + logic.data.undoStackIndex
                        + ") is larger than undoStack length (" + logic.data.undoStack.length + ")")
            console.assert(logic.data.undoStackIndex >= 0,
                           "Current undoStack index is < 0")
            if (typeof redoFunctor === "function"
                    && typeof undoFunctor === "function") {
                if (logic.data.undoStackIndex < logic.data.undoStack.length) {
                    let test = logic.data.undoStack.splice(
                            logic.data.undoStackIndex)
                }
                logic.data.undoStack.push({
                                              "undo": undoFunctor,
                                              "redo": redoFunctor
                                          })
                redo()
            }
        }

        function index() {
            return logic.data.undoStackIndex
        }

        function setIndex(index) {

            let currentIndex = logic.data.undoStackIndex
            let toIndex = Math.max(Math.min(logic.data.undoStack.length,
                                            index), 0)

            if (toIndex < currentIndex) {
                console.debug(
                            "Undoing from index " + currentIndex + " to index " + toIndex)
                for (var counter = currentIndex; counter > toIndex; counter--) {
                    undo()
                }
            } else if (toIndex > currentIndex) {
                console.debug(
                            "Redoing from index " + currentIndex + " to index " + toIndex)
                for (var counter = currentIndex; counter < toIndex; counter++) {
                    redo()
                }
            }
        }

        function undo() {
            if (logic.data.undoStackIndex > 0) {

                console.assert(
                            logic.data.undoStackIndex <= logic.data.undoStack.length,
                            "Current undoStack index is larger than undoStack length")
                console.assert(logic.data.undoStackIndex >= 0,
                               "Current undoStack index is < 0")
                logic.data.undoStackIndex--
                console.assert(
                            typeof logic.data.undoStack[logic.data.undoStackIndex].undo
                            === "function", "Undo command not a functor")
                logic.data.undoStack[logic.data.undoStackIndex].undo()
                console.debug("Called undo()")
            }
        }

        function redo() {
            if (logic.data.undoStackIndex < logic.data.undoStack.length) {
                console.assert(
                            logic.data.undoStackIndex <= logic.data.undoStack.length,
                            "Current undoStack index is larger than undoStack length")
                console.assert(logic.data.undoStackIndex >= 0,
                               "Current undoStack index is < 0")
                console.assert(
                            typeof logic.data.undoStack[logic.data.undoStackIndex].redo
                            === "function", "Undo command not a functor")
                logic.data.undoStack[logic.data.undoStackIndex].redo()
                logic.data.undoStackIndex++
                console.debug("Called redo()")
            }
        }

        function toModelKey(row, column) {
            return "row" + row + "-column" + column
        }

        function copyData() {

            let tilesCopy = {}
            if (logic.data.tiles) {
                for (let key in logic.data.tiles) {
                    let tile = logic.data.tiles[key]
                    if (tile && tile.hasOwnProperty("id")
                            && tile.hasOwnProperty("row")
                            && tile.hasOwnProperty("column")
                            && tile.hasOwnProperty("rowSpan")
                            && tile.hasOwnProperty("columnSpan")) {
                        tilesCopy[key] = newTile(tile.row, tile.column,
                                                 tile.rowSpan, tile.columnSpan)
                        tilesCopy[key].id = tile.id
                    }
                }
            }

            let component = Qt.createComponent(Qt.resolvedUrl(
                                                   "TileLayoutData.qml"))
            let data = component.createObject(null, {
                                                  "rows": logic.data.rows,
                                                  "columns": logic.data.columns,
                                                  "undoStackIndex": logic.data.undoStackIndex,
                                                  "undoStack": logic.data.undoStack.slice(
                                                                   ),
                                                  "tiles": tilesCopy
                                              })
            if (data == null) {
                console.log("Error copying tile layout data")
            }

            return data
        }

        function uuidv4() {
            return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g,
                                                                  function (c) {
                                                                      var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8)
                                                                      return v.toString(16)
                                                                  })
        }

        function newTile(row, column, rowSpan, columnSpan) {

            return {
                "id": uuidv4(),
                "row": row,
                "column": column,
                "rowSpan": rowSpan,
                "columnSpan": columnSpan,
                "toString": function () {
                    return "Tile(row " + this.row + ", col " + this.column
                            + ", rowSpan " + this.rowSpan + ", columnSpan "
                            + this.columnSpan + ", id " + this.id + ")"
                },
                "equals": function (other) {
                    return other && this.row === other.row
                            && this.column === other.column
                            && this.rowSpan === other.rowSpan
                            && this.columnSpan === other.columnSpan
                }
            }
        }
    }
}
