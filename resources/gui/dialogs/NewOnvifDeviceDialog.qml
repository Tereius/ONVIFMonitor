import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QuickFuture 1.0
import org.onvif.device 1.0
import org.kde.kirigami 2.14 as Kirigami
import "../controls" as Controls

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
            text: "Add"
            icon.name: "check"
            icon.color: "red"
            enabled: deviceNameField.acceptableInput
                     && hostField.acceptableInput
            onTriggered: {
                priv.addDevice()
            }
        }
    ]

    Controls.ScrollablePage {

        anchors.fill: parent
        Keys.onEnterPressed: priv.addDevice()
        Keys.onReturnPressed: priv.addDevice()

        ColumnLayout {

            width: parent.width

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
                    alert: true
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

                    Controls.TextField {

                        id: deviceNameField

                        text: credentialsDialog.deviceName
                        focus: true
                        Layout.fillWidth: true
                        activeFocusOnTab: true
                        placeholderText: "Device Name"
                        selectByMouse: true
                        enabled: !credentialsDialog.deviceNameFixed
                        validator: RegExpValidator {
                            regExp: /.+/
                        }
                    }

                    Controls.TextField {

                        id: hostField

                        text: credentialsDialog.deviceEndpoint
                        Layout.fillWidth: true
                        activeFocusOnTab: true
                        placeholderText: "Device Host"
                        selectByMouse: true
                        enabled: !credentialsDialog.deviceEndpointFixed
                        validator: RegExpValidator {
                            regExp: /.+/
                        }
                    }

                    Controls.TextField {

                        id: userField

                        Layout.fillWidth: true
                        placeholderText: qsTr("User")
                        activeFocusOnTab: true
                        selectByMouse: true
                    }

                    Controls.TextField {

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
                    credentialsDialog.deviceId ? credentialsDialog.deviceId : App.createUuid(
                                                     ))
            Future.onFinished(future, function (result) {
                console.warn(result.getDetails())
                if (result.isSuccess()) {
                    credentialsDialog.close()
                } else {
                    message.pushMessage(result.getDetails())
                }
                credentialsDialog.busy = false
            }, function () {
                credentialsDialog.busy = false
            })
        }
    }
}
