import QtQuick 2.12
import QtQuick.Controls 2.12 as T
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.12
import "../helper.js" as Helper

T.GroupBox {

    id: control

    property BusyAction mainAction
    property string infoText: ""
    property alias icon: iconLabel.icon

    background: Rectangle {
        width: parent.width
        height: parent.height
        color: "#393942"
    }

    topPadding: padding + control.implicitLabelHeight

    Behavior on implicitHeight {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutCubic
        }
    }

    label: Item {

        x: control.leftPadding
        visible: control.title.length > 0 || control.mainAction
        implicitWidth: visible ? control.availableWidth : 0
        implicitHeight: visible ? control.Material.delegateHeight - 4 : 0

        RowLayout {

            id: row
            anchors.fill: parent

            Icon {
                id: iconLabel
            }

            Item {

                Layout.fillWidth: true

                T.Label {
                    id: label
                    text: control.title
                    elide: Text.ElideRight
                    anchors.verticalCenter: parent.verticalCenter
                    width: Math.min(
                               implicitWidth,
                               parent.width - (icon.visible ? icon.width : 0) - 2)
                }

                Icon {
                    id: icon
                    visible: control.infoText.length > 0
                    icon.name: "information-outline"
                    icon.width: 16
                    icon.height: 16
                    anchors.bottom: label.baseline
                    anchors.bottomMargin: -3
                    anchors.left: label.right
                    anchors.leftMargin: 2

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            const dialog = Helper.createDialog(
                                             "controls/InfoDialog.qml",
                                             T.ApplicationWindow.window, {
                                                 "text": control.infoText
                                             })
                        }
                    }
                }
            }

            T.ToolButton {
                id: actionButton

                action: control.mainAction
                leftPadding: 0
                rightPadding: 0
                visible: action
                enabled: control.mainAction ? !control.mainAction.busy : false
                implicitHeight: parent.height

                font.capitalization: Font.AllUppercase
                font.styleName: "Bold"
                font.letterSpacing: 2.8

                background: Rectangle {

                    T.Label {
                        // TextMetrics does not work, retrns wrong width
                        id: text
                        text: actionButton.text
                        font: actionButton.font
                        visible: false
                    }

                    color: actionButton.icon.color
                    anchors.bottom: actionButton.contentItem.bottom
                    anchors.bottomMargin: 5
                    anchors.right: actionButton.contentItem.right

                    width: text.implicitWidth
                    height: actionButton.hovered && actionButton.enabled ? 2 : 0

                    antialiasing: true

                    Behavior on height {
                        enabled: actionButton.enabled
                        SmoothedAnimation {
                            duration: 250
                            velocity: -1
                        }
                    }
                }
            }
        }

        Rectangle {
            id: devider
            width: control.availableWidth
            color: control.Material.backgroundColor
            implicitHeight: 2
            anchors.top: row.bottom

            T.ProgressBar {

                anchors.fill: parent

                visible: control.mainAction ? control.mainAction.busy : false

                indeterminate: true

                Component.onCompleted: {
                    contentItem.implicitHeight = 2
                }
            }
        }
    }
}
