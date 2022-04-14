import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.12
import QtQuick.Controls.Material.impl 2.12

GroupBox {

    id: control

    property bool fold: true
    property string text: ""
    property alias count: messageModel.count

    // severity one of "error", "warning", "info"
    function pushMessage(message, severity, title) {

        if (!severity) {
            severity = "info"
        }

        messageModel.insert(0, {
                                "title": title,
                                "severity": severity,
                                "displayMessage": message
                            })
        //propertyAnimIn.start()
    }

    function hide() {//propertyAnimIn.stop()
        //propertyAnimOut.start()
    }

    label: Item {}

    Behavior on implicitHeight {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    ListModel {
        id: messageModel
    }

    ColumnLayout {

        id: columLayout
        width: parent.width
        spacing: 10

        RowLayout {
            Layout.minimumHeight: 22
            Layout.maximumHeight: 22
            Layout.fillWidth: true

            Repeater {
                model: messageModel

                Row {
                    visible: index === 0
                    spacing: 6
                    Layout.fillWidth: true
                    Icon {
                        anchors.verticalCenter: parent.verticalCenter
                        icon.name: {

                            switch (severity) {
                            case "error":
                                return "ic_warning"
                            case "warning":
                                return "ic_warning"
                            case "info":
                                return "information"
                            default:
                                return "information"
                            }
                        }
                        icon.color: {

                            switch (severity) {
                            case "error":
                                return Material.color(Material.Red)
                            case "warning":
                                return Material.color(Material.Yellow)
                            case "info":
                                return Material.accentColor
                            default:
                                return Material.accentColor
                            }
                        }
                    }

                    T.Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: {

                            if (!title) {
                                switch (severity) {
                                case "error":
                                    return qsTr("Error")
                                case "warning":
                                    return qsTr("Warning")
                                case "info":
                                    return qsTr("Info")
                                default:
                                    return ""
                                }
                            } else {
                                return title
                            }
                        }
                    }
                }
            }

            Item {

                implicitWidth: button.implicitWidth
                implicitHeight: 22
                Layout.fillHeight: true

                T.ToolButton {
                    id: button
                    visible: messageModel.count > grid.columns
                    text: control.fold ? qsTr("SEE ALL") + " (" + messageModel.count + ")" : qsTr(
                                             "SEE LESS")
                    anchors.verticalCenter: parent.verticalCenter
                    implicitHeight: 22 + control.topPadding + columLayout.spacing

                    onClicked: {
                        control.fold = !control.fold
                    }
                }
            }
        }

        Rectangle {
            height: 2
            Layout.fillWidth: true
            color: control.Material.backgroundColor
        }

        Grid {

            id: grid

            property real minWidth: (parent.width - (grid.columns - 1)
                                     * columnSpacing) / grid.columns

            columns: Math.max(parent.width / 400, 1)
            columnSpacing: 50
            rowSpacing: 10

            add: Transition {
                SequentialAnimation {
                    PauseAnimation {
                        duration: grid.columns > 1 ? 100 : 0
                    }
                    ParallelAnimation {
                        NumberAnimation {
                            property: "opacity"
                            from: 0
                            to: 1.0
                            duration: 300
                        }
                        ScaleAnimator {
                            from: 0.85
                            to: 1
                            duration: 300
                            running: false
                            easing.type: Easing.OutQuad
                        }
                    }
                }
            }

            move: Transition {
                NumberAnimation {
                    properties: "x,y"
                    duration: 300
                    easing.type: Easing.OutCubic
                }
            }

            Repeater {

                id: rep
                model: messageModel

                ColumnLayout {

                    width: grid.minWidth
                    spacing: 10
                    visible: control.fold ? index < grid.columns : true
                    opacity: 0

                    RowLayout {

                        Layout.fillWidth: true

                        T.Label {
                            text: displayMessage
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                            Layout.topMargin: 4
                            Layout.alignment: Qt.AlignTop
                        }

                        T.RoundButton {
                            icon.name: "playlist-remove"
                            flat: true
                            Layout.alignment: Qt.AlignTop
                            Layout.topMargin: -10
                            onClicked: {
                                messageModel.remove(index)
                            }
                        }
                    }

                    Rectangle {
                        height: 2
                        color: control.Material.backgroundColor
                        Layout.fillWidth: true
                    }
                }
            }
        }

        T.Label {
            visible: messageModel.count === 0
            text: qsTr("No messages")
        }
    }
}
