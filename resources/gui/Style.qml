import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QuickFuture 1.0
import QtQuick.Controls.Material 2.12
import org.onvif.device 1.0
import "helper.js" as Helper
import "controls" as Controls

Controls.ScrollablePage {

    Column {

        padding: 0
        spacing: 0

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

        Controls.Button {
            text: "Show popup"

            onClicked: {
                popup.openWithAnimOffset(mapToItem(Overlay.overlay, 0,
                                                   pressY).y)
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

        Label {
            text: "font NativeRendering"
            font.pixelSize: 20
            font.styleName: "Light"
            renderType: Text.NativeRendering
        }

        Label {
            text: "font QtRendering"
            font.pixelSize: 20
            font.styleName: "Light"
            renderType: Text.QtRendering
        }

        Label {
            text: "font default rendering"
            font.pixelSize: 20
            font.styleName: "Light"
        }

        ComboBox {
            model: ListModel {
                id: model
                ListElement {
                    text: "Text.NativeRendering"
                    value: Text.NativeRendering
                }

                ListElement {
                    text: "Text.QtRendering"
                    value: Text.QtRendering
                }
            }

            textRole: "text"
            valueRole: "value"

            onActivated: {
                fontsRow.renderType = model.get(index).value
            }
        }

        Row {

            id: fontsRow
            spacing: 10

            property var renderType: Text.NativeRendering

            Column {

                Label {
                    text: "font default"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight Thin"
                    font.weight: Font.Thin
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight ExtraLight"
                    font.weight: Font.ExtraLight
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight Light"
                    font.weight: Font.Light
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight Normal"
                    font.weight: Font.Normal
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight Medium"
                    font.weight: Font.Medium
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight DemiBold"
                    font.weight: Font.DemiBold
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight Bold"
                    font.weight: Font.Bold
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight ExtraBold"
                    font.weight: Font.ExtraBold
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font weight Black"
                    font.weight: Font.Black
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }
            }

            Column {
                Label {
                    text: "font default"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style Thin"
                    font.styleName: "Thin"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style ExtraLight"
                    font.styleName: "ExtraLight"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style Light"
                    font.styleName: "Light"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style Regular"
                    font.styleName: "Regular"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style Medium"
                    font.styleName: "Medium"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style DemiBold"
                    font.styleName: "DemiBold"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style Bold"
                    font.styleName: "Bold"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style ExtraBold"
                    font.styleName: "ExtraBold"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }

                Label {
                    text: "font style Black"
                    font.styleName: "Black"
                    font.pixelSize: 20
                    renderType: fontsRow.renderType
                }
            }
        }
    }
}
