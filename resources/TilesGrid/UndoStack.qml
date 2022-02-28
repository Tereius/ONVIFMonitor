import QtQuick 2.12

QtObject {

    id: stack

    readonly property int index: stack._private.undoStackIndex

    function pushUndoRedoCommand(redoFunctor, undoFunctor) {

        console.assert(typeof redoFunctor === "function",
                       "redoFunctor not a function")
        console.assert(typeof undoFunctor === "function",
                       "undoFunctor not a function")
        console.assert(
                    stack._private.undoStackIndex <= stack._private.undoStack.length,
                    "Current undoStack index (" + stack._private.undoStackIndex
                    + ") is larger than undoStack length (" + stack._private.undoStack.length + ")")
        console.assert(stack._private.undoStackIndex >= 0,
                       "Current undoStack index is < 0")
        if (typeof redoFunctor === "function"
                && typeof undoFunctor === "function") {
            if (stack._private.undoStackIndex < stack._private.undoStack.length) {
                let test = stack._private.undoStack.splice(
                        stack._private.undoStackIndex)
            }
            stack._private.undoStack.push({
                                              "undo": undoFunctor,
                                              "redo": redoFunctor
                                          })
            redo()
        }
    }

    function index() {

        return stack._private.undoStackIndex
    }

    function setIndex(index) {

        let currentIndex = stack._private.undoStackIndex
        let toIndex = Math.max(Math.min(stack._private.undoStack.length,
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

        if (stack._private.undoStackIndex > 0) {
            console.assert(
                        stack._private.undoStackIndex <= stack._private.undoStack.length,
                        "Current undoStack index is larger than undoStack length")
            console.assert(stack._private.undoStackIndex >= 0,
                           "Current undoStack index is < 0")
            stack._private.undoStackIndex--
            console.assert(
                        typeof stack._private.undoStack[stack._private.undoStackIndex].undo
                        === "function", "Undo command not a functor")
            stack._private.undoStack[stack._private.undoStackIndex].undo()
            console.debug("Called undo()")
        }
    }

    function redo() {

        if (stack._private.undoStackIndex < stack._private.undoStack.length) {
            console.assert(
                        stack._private.undoStackIndex <= stack._private.undoStack.length,
                        "Current undoStack index is larger than undoStack length")
            console.assert(stack._private.undoStackIndex >= 0,
                           "Current undoStack index is < 0")
            console.assert(
                        typeof stack._private.undoStack[stack._private.undoStackIndex].redo
                        === "function", "Undo command not a functor")
            stack._private.undoStack[stack._private.undoStackIndex].redo()
            stack._private.undoStackIndex++
            console.debug("Called redo()")
        }
    }

    readonly property QtObject _private: QtObject {

        property int undoStackIndex: 0
        property var undoStack: []
    }
}
