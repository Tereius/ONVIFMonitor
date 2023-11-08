import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Onvif

Page {

    id: page

    property var deviceId

    title: qsTr("Event Handling")

    EventBindingModel {
        id: eventBindingModel
    }

    ExpandableListView {

        id: bindingListView

        height: contentHeight
        width: parent.width
        interactive: false

        model: eventBindingModel
        delegate: ListItem {

            id: bindingItem

            width: parent.width

            label: name
            text: description

            content: Item {

                height: row.height
                width: parent.width

                RowLayout {

                    id: row
                    width: parent.width
                    anchors.centerIn: parent

                    RoundButton {
                        text: qsTr("Source")

                        onClicked: {
                            var component = Qt.createComponent(
                                        "EventPropertiesDialog.qml")
                            var dialog = component.createObject(page, {
                                                                    "bindingId": bindingId,
                                                                    "handler": false
                                                                })
                            dialog.onClosed.connect(function () {
                                dialog.destroy()
                            })
                            dialog.open()
                        }

                        background: rectangel

                        RectangularGlow {
                            visible: parent.hovered
                            z: -2
                            color: hasSource ? Universal.color(
                                                   Universal.Green) : Universal.color(
                                                   Universal.Red)
                            anchors.fill: parent
                            glowRadius: 5
                            spread: 0
                            cornerRadius: rectangel.radius
                        }

                        Rectangle {
                            id: rectangel
                            radius: height / 2
                            border.width: 2
                            border.color: hasSource ? Universal.color(
                                                          Universal.Green) : Universal.color(
                                                          Universal.Red)
                            color: "#33000000"
                        }
                    }

                    Rectangle {
                        width: 75
                        height: 2
                        color: "black"
                        anchors.verticalCenter: row.verticalCenter
                        Layout.minimumWidth: 5
                        Layout.fillWidth: true
                    }

                    RoundButton {

                        implicitHeight: 50
                        implicitWidth: 50

                        anchors.verticalCenter: row.verticalCenter

                        icon.name: "link"
                        onClicked: {

                            var result = EventManager.triggerHandler(bindingId)
                            if (!result.isSuccess())
                                window.showError("error", result.toString())
                        }

                        PingAnimation {
                            id: pingAnim
                            anchors.fill: parent

                            Connections {
                                enabled: bindingItem.highlighted
                                target: EventManager
                                onEventOccured: {
                                    if (rBindingId === bindingId)
                                        pingAnim.ping()
                                }
                            }
                        }
                    }

                    Rectangle {
                        width: 75
                        height: 2
                        color: "black"
                        anchors.verticalCenter: row.verticalCenter
                        Layout.minimumWidth: 5
                        Layout.fillWidth: true
                    }

                    RoundButton {
                        text: qsTr("Handler")

                        onClicked: {
                            var component = Qt.createComponent(
                                        "EventPropertiesDialog.qml")
                            var dialog = component.createObject(page, {
                                                                    "bindingId": bindingId,
                                                                    "handler": true
                                                                })
                            dialog.onClosed.connect(function () {
                                dialog.destroy()
                            })
                            dialog.open()
                        }
                    }
                }
            }
        }

        header: RowLayout {

            width: parent.width
            height: 50

            spacing: 10

            Label {

                Layout.fillWidth: true

                elide: Text.ElideRight
                text: qsTr("Events")
                font.pixelSize: 15
            }

            Button {

                text: qsTr("Add")
                icon.name: "plus"

                onClicked: {

                    newEventBindingDialog.open()
                }
            }
        }
    }

    NewEventBindingDialog {

        id: newEventBindingDialog
    }
}
