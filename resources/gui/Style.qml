import QtQuick 2.10
import QtQml 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QuickFuture 1.0
import QtQuick.Controls.Material 2.12
import org.onvif.device 1.0
import org.kde.kirigami 2.14 as Kirigami
import "helper.js" as Helper
import "controls" as Controls

Kirigami.ScrollablePage {

    Controls.Popup {
        id: popup

        Label {
            text: "Lorem ipsum"
        }
    }

    Column {

        spacing: 10

        Controls.Button {
            text: "Show popup"

            onClicked: {
                popup.openWithAnimOffset(mapToItem(Overlay.overlay, 0,
                                                   pressY).y)
            }
        }

        Controls.GroupBox {
            title: "Test"
            Column {

                Switch {}

                TextField {
                    placeholderText: "IP-Adresse"
                }

                Controls.TextField {
                    placeholderText: "IP-Adresse"
                    text: "Thisdflj asdlfkj asdlfkj asdlfkj asldkfja sldfj asdf"
                }

                Controls.TextField {
                    enabled: false
                    placeholderText: "IP-Adresse"
                    text: "asdfaefw"
                }
            }
        }

        Controls.ComboBox {
            model: ["Webcam", "Generic", "ONVIFsdf asgsa fas dfdsdf"]
            editable: false
            placeholderText: "IP-Adresse"
        }

        Controls.ComboBox {
            model: ["Webcam", "Generic", "ONVIFsdfgsfd"]
            editable: true
        }

        ComboBox {
            model: ["Webcam", "Generic", "ONVIFsdfgds"]
            editable: true
        }

        BusyIndicator {}

        Controls.Button {
            text: "Button"
            onClicked: {
                popup.openWithAnimOffset(mapToItem(Overlay.overlay, 0,
                                                   pressY).y)
            }
        }

        Controls.Button {
            text: "Button"
            icon.name: "ic_delete"
        }

        Controls.Button {
            text: "Button with a lot of text"
            icon.name: "ic_delete"
        }

        Controls.Button {
            enabled: false
            text: "Disabled Button"
            icon.name: "ic_delete"
        }

        CheckBox {}

        DelayButton {
            text: "Delay"
        }

        Dial {}

        RangeSlider {
            from: 1
            to: 100
            first.value: 25
            second.value: 75
        }

        RoundButton {
            text: "Round"
        }

        TabBar {
            TabButton {
                text: "First"
            }
            TabButton {
                text: "Second"
            }
        }

        RoundButton {
            text: "Tooltip"
            ToolTip.visible: down
            ToolTip.text: qsTr("Save the active project")
        }

        ToolBar {
            RowLayout {
                anchors.fill: parent
                ToolButton {
                    text: qsTr("‹")
                    onClicked: stack.pop()
                }
                Label {
                    text: "Title"
                    elide: Label.ElideRight
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                }
                ToolButton {
                    text: qsTr("⋮")
                    onClicked: menu.open()
                }
            }
        }
    }
}
