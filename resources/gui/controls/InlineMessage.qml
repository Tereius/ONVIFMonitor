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

    property T.Action action: T.Action {

        //visible: messageModel.count > grid.columns
        text: control.fold ? qsTr("SEE ALL") + " (" + messageModel.count + ")" : qsTr(
                                 "SEE LESS")

        onTriggered: {
            control.fold = !control.fold
        }
    }

    mainAction: messageModel.count > grid.columns ? action : null

    Behavior on implicitHeight {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    ListModel {
        id: messageModel

        onCountChanged: {

            if (messageModel.count > 0) {
                control.title = messageModel.get(0).title
                const severity = messageModel.get(0).severity
                switch (severity) {
                case "error":
                    control.icon.name = "ic_warning"
                    control.icon.color = control.Material.color(Material.Red)
                    break
                case "warning":
                    control.icon.name = "ic_warning"
                    control.icon.color = control.Material.color(Material.Yellow)
                    break
                case "info":
                    control.icon.name = "information"
                    control.icon.color = control.Material.accentColor
                    break
                default:
                    control.icon.name = "information"
                    control.icon.color = control.Material.accentColor
                }
            } else {
                control.title = ""
            }
        }
    }

    ColumnLayout {

        id: columLayout
        width: parent.width
        spacing: 10

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
                        height: 1
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
