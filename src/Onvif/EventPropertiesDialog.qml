import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Onvif

Dialog {

    id: settingsDialog

    property var bindingId
    property bool handler

    x: Math.round((window.width - width) / 2)
    y: Math.round(window.height / 6)
    width: Math.round(Math.min(window.width, window.height) / 3 * 2)
    modal: true
    focus: true
    title: handler ? qsTr("Event Handler") : qsTr("Event Source")

    standardButtons: Dialog.Ok | Dialog.Cancel

    QtObject {
        id: d
        readonly property var eventBindingInfo: eventBindingModel.get(bindingId)
        property string eventPropertyType
        property var properties: []
        property var propertyItems: []
        property var propertiesRet: {

        }

        onPropertiesChanged: {

            propertiesRet = {}

            for (var i = propertyItems.length - 1; i >= 0; i--) {
                propertyItems[i].destroy()
                propertyItems.splice(i, 1)
            }

            if (properties) {

                properties.forEach(function (prop) {

                    var valueItem

                    if (prop.name) {
                        propertyItems.push(
                                    Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; Label {text: "'
                                        + prop.name + '"}', form))

                        if (settingsDialog.handler) {
                            if (eventBindingInfo
                                    && eventBindingInfo.eventHandlerProperties
                                    && eventBindingInfo.eventHandlerProperties[prop.name]) {
                                propertiesRet[prop.name]
                                        = eventBindingInfo.eventHandlerProperties[prop.name]
                            } else {
                                propertiesRet[prop.name] = prop.defaultValue
                            }
                        } else {
                            if (eventBindingInfo
                                    && eventBindingInfo.eventSourceProperties
                                    && eventBindingInfo.eventSourceProperties[prop.name]) {
                                propertiesRet[prop.name]
                                        = eventBindingInfo.eventSourceProperties[prop.name]
                            } else {
                                propertiesRet[prop.name] = prop.defaultValue
                            }
                        }

                        if (prop.type === PropertyInfo.UNKNOWN) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; Label {text: "Unknown Type"}',
                                        form)
                        } else if (prop.type === PropertyInfo.POSITIVE_INTEGER) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; SpinBox {editable: true; from: 0; to: 10000}',
                                        form)

                            valueItem.value = propertiesRet[prop.name]
                            valueItem.onValueChanged.connect(function () {
                                propertiesRet[prop.name] = valueItem.value
                            })
                        } else if (prop.type === PropertyInfo.INTEGER) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; SpinBox {editable: true; from: -10000; to: 10000}',
                                        form)

                            valueItem.value = propertiesRet[prop.name]
                            valueItem.onValueChanged.connect(function () {
                                propertiesRet[prop.name] = valueItem.value
                            })
                        } else if (prop.type === PropertyInfo.DECIMAL) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; SpinBox {editable: true; from: -10000; to: 10000; stepSize: 0.1}',
                                        form)

                            valueItem.value = propertiesRet[prop.name]
                            valueItem.onValueChanged.connect(function () {
                                propertiesRet[prop.name] = valueItem.value
                            })
                        } else if (prop.type === PropertyInfo.STRING) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; TextField {}',
                                        form)

                            valueItem.text = propertiesRet[prop.name]
                            valueItem.editingFinished.connect(function () {
                                propertiesRet[prop.name] = valueItem.text
                            })
                        } else if (prop.type === PropertyInfo.BOOL) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; Switch {}',
                                        form)

                            valueItem.checked = propertiesRet[prop.name]
                            valueItem.toggled.connect(function () {
                                propertiesRet[prop.name] = valueItem.checked
                            })
                        } else if (prop.type === PropertyInfo.DATE) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; Label {text: "PropertyInfo.DATE"}',
                                        form)
                        } else if (prop.type === PropertyInfo.DATE_TIME) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; Label {text: "PropertyInfo.DATE_TIME"}',
                                        form)
                        } else if (prop.type === PropertyInfo.STRING_LIST) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; ComboBox {}',
                                        form)

                            valueItem.model = propertiesRet[prop.name]
                            valueItem.onCurrentTextChanged.connect(function () {
                                propertiesRet[prop.name] = valueItem.currentText
                            })
                        } else if (prop.type === PropertyInfo.ONVIF_MESSAGE_FILTER) {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick 2.0; OnvifMessageFilterBuilder { height: 500}',
                                        form)

                            //valueItem.model = propertiesRet[prop.name]
                            //valueItem.onCurrentTextChanged.connect(function () {
                            //	propertiesRet[prop.name] = valueItem.currentText
                            //})
                        } else {
                            valueItem = Qt.createQmlObject(
                                        'import QtQuick.Controls 2.3; TextField {}',
                                        form)

                            valueItem.text = propertiesRet[prop.name]
                            valueItem.editingFinished.connect(function () {
                                propertiesRet[prop.name] = valueItem.text
                            })
                        }
                        valueItem.Layout.fillWidth = true
                        propertyItems.push(valueItem)
                    }
                })
            }
        }
    }

    onAccepted: {

        if (settingsDialog.handler) {
            if (d.eventPropertyType)
                EventManager.bindHandler(bindingId, d.eventPropertyType,
                                         d.propertiesRet)
            else
                EventManager.unbindHandler(bindingId)
        } else {
            if (d.eventPropertyType)
                EventManager.bindSource(bindingId, d.eventPropertyType,
                                        d.propertiesRet)
            else
                EventManager.unbindSource(bindingId)
        }
    }

    onRejected: {

        settingsDialog.close()
    }

    EventSourceModel {

        id: eventSourceModel
    }

    EventHandlerModel {

        id: eventHandlerModel
    }

    EventBindingModel {

        id: eventBindingModel
    }

    contentItem: GridLayout {

        id: form

        columns: 2

        RowLayout {

            spacing: parent.columnSpacing
            Layout.fillWidth: true
            Layout.columnSpan: 2

            ComboBox {

                id: eventSourceSelector

                Layout.fillWidth: true

                model: settingsDialog.handler ? eventHandlerModel : eventSourceModel
                textRole: "name"
                currentIndex: -2

                Component.onCompleted: {

                    if (settingsDialog.handler) {
                        if (d.eventBindingInfo
                                && d.eventBindingInfo.eventHandler)
                            currentIndex = find(d.eventBindingInfo.eventHandler)
                        else
                            currentIndex = -1
                    } else {
                        if (d.eventBindingInfo
                                && d.eventBindingInfo.eventSource)
                            currentIndex = find(d.eventBindingInfo.eventSource)
                        else
                            currentIndex = -1
                    }
                }

                onCurrentIndexChanged: {

                    if (currentIndex >= 0) {
                        var modelData = settingsDialog.handler ? eventHandlerModel.get(
                                                                     currentIndex) : eventSourceModel.get(
                                                                     currentIndex)
                        hostField.text = modelData.description
                        d.eventPropertyType = modelData.value
                        d.properties = modelData.properties
                    } else {
                        hostField.text = settingsDialog.handler ? qsTr("Please select an event handler") : qsTr("Please select an event source")
                        d.eventPropertyType = ""
                        d.properties = undefined
                    }
                }
            }

            Button {

                icon.name: "close"
                enabled: eventSourceSelector.currentIndex >= 0
                onClicked: {
                    eventSourceSelector.currentIndex = -1
                }
            }
        }

        Text {

            id: hostField

            Layout.fillWidth: true

            Layout.columnSpan: 2
            wrapMode: Text.WordWrap
        }
    }
}
