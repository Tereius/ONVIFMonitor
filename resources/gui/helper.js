.pragma library
.import QtQml 2.12 as QtQml

function createDialog(url, parent, options) {

    const dialog = createItem(Qt.resolvedUrl(url), parent, options)

    if (typeof dialog.open === "function") {
        dialog.open()
    }

    if (typeof dialog.closed === "function") {
        dialog.closed.connect(function () {
            console.warn("-------ddddd")
            dialog.destroy()
        })
    }

    return dialog
}

function createItem(url, parent, options) {

    let component = Qt.createComponent(Qt.resolvedUrl(url))

    let finishCreation = function () {
        if (component.status == QtQml.Component.Ready) {
            if (!options) {
                options = {}
            }

            let item = component.createObject(parent, options)
            if (item == null) {
                // Error Handling
                console.warn("Error creating object")
            }
            return item
        } else if (component.status == QtQml.Component.Error) {
            // Error Handling
            console.warn("Error loading component:", component.errorString())
        }
    }

    if (component.status == QtQml.Component.Ready)
        return finishCreation()
    else if (component.status == QtQml.Component.Loading)
        component.statusChanged.connect(finishCreation)
    else
        console.warn("Error creating Item: " + component.errorString())
}

function markRejectedLabel(text, acceptedDecisionProperty) {

    return !acceptedDecisionProperty ? "<font color=\"red\">" + text + "</font>" : text
}
