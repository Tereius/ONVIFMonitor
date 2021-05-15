.pragma library
.import QtQml 2.12 as QtQml

function createItem(url, parent, options) {

    let component = Qt.createComponent(url)

    let finishCreation = function () {
        if (component.status == QtQml.Component.Ready) {
            if (!options) {
                options = {}
            }

            let item = component.createObject(parent, options)
            if (item == null) {
                // Error Handling
                console.log("Error creating object")
            }
            return item
        } else if (component.status == QtQml.Component.Error) {
            // Error Handling
            console.log("Error loading component:", component.errorString())
        }
    }

    if (component.status == QtQml.Component.Ready)
        return finishCreation()
    else
        component.statusChanged.connect(finishCreation)
}

function markRejectedLabel(text, acceptedDecisionProperty) {

    return !acceptedDecisionProperty ? "<font color=\"red\">" + text + "</font>" : text
}
