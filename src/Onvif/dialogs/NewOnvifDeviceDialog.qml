import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QuickFuture
import Onvif
import MaterialRally as Controls

Controls.Popup {

    title: qsTr("Add Device")

    id: credentialsDialog
    property string deviceName: ""
    property string deviceEndpoint: ""
    property var deviceId: null
    property bool deviceNameFixed: false
    property bool deviceEndpointFixed: false

    actions: [
        Controls.BusyAction {
            id: addAction
            text: qsTr("Add")
            icon.name: "check"
            icon.color: "red"
            enabled: deviceNameField.acceptableInput
                     && deviceNameField.text.length > 0
                     && hostField.acceptableInput && hostField.text.length > 0
            onTriggered: {
                priv.addDevice()
            }
        }
    ]

    ScrollView {

        anchors.fill: parent
        Keys.onEnterPressed: priv.addDevice()
        Keys.onReturnPressed: priv.addDevice()

        ColumnLayout {

            width: credentialsDialog.width

            Item {

                Layout.fillWidth: true
                implicitHeight: message.count === 0 ? 0 : message.implicitHeight

                Behavior on implicitHeight {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.OutCubic
                    }
                }

                Controls.InlineMessage {
                    id: message
                    title: qsTr("Error")
                    width: parent.width

                    Behavior on implicitHeight {
                        enabled: false
                    }
                }
            }

            Controls.GroupBox {

                Layout.fillWidth: true

                ColumnLayout {

                    spacing: 20
                    anchors.fill: parent

                    TextField {

                        id: deviceNameField

                        text: credentialsDialog.deviceName
                        focus: true
                        Layout.fillWidth: true
                        activeFocusOnTab: true
                        placeholderText: qsTr("Device Name")
                        selectByMouse: true
                        enabled: !credentialsDialog.deviceNameFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    TextField {

                        id: hostField

                        text: credentialsDialog.deviceEndpoint
                        Layout.fillWidth: true
                        activeFocusOnTab: true
                        placeholderText: qsTr("Device Host")
                        selectByMouse: true
                        enabled: !credentialsDialog.deviceEndpointFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    TextField {

                        id: userField

                        Layout.fillWidth: true
                        placeholderText: qsTr("User")
                        activeFocusOnTab: true
                        selectByMouse: true
                    }

                    TextField {

                        id: passwordField

                        echoMode: TextInput.Password
                        Layout.fillWidth: true
                        placeholderText: qsTr("Password")
                        activeFocusOnTab: true
                        selectByMouse: true
                    }
                }
            }
        }
    }

    QtObject {
        id: priv

        function addDevice() {
            credentialsDialog.busy = true
            let future = DeviceManager.addDevice(
                    hostField.text, userField.text, passwordField.text,
                    deviceNameField.text,
                    credentialsDialog.deviceId ? credentialsDialog.deviceId : Onvif.createUuid(
                                                     ))
            Future.onFinished(future, function (result) {

                if (result.isSuccess()) {
                    credentialsDialog.close()
                } else {
                    message.pushMessage(result.getDetails(), "error", "Error")
                }
                credentialsDialog.busy = false
            }, function () {
                credentialsDialog.busy = false
            })
        }
    }
}
