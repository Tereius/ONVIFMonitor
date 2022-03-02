import QtQuick 2.10
import QtQml 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QuickFuture 1.0
import org.onvif.device 1.0
import org.kde.kirigami 2.14 as Kirigami
import "helper.js" as Helper
import "controls" as Controls

Controls.ScrollablePage {

    id: credentialsDialog
    property string deviceName: ""
    property string deviceEndpoint: ""
    property var deviceId: null
    property bool deviceNameFixed: false
    property bool deviceEndpointFixed: false

    title: qsTr("Add Device")
    supportsRefreshing: true

    Keys.onEnterPressed: priv.addDevice()
    Keys.onReturnPressed: priv.addDevice()

    signal accepted


    /*
    actions.main: Kirigami.Action {

        id: mainAction
        property bool isRunning: false

        displayComponent: Component {
            ToolButton {
                text: qsTr("Add")
                enabled: !mainAction.isRunning
                         && deviceNameField.acceptableInput
                         && hostField.acceptableInput
                icon.name: mainAction.isRunning ? "ic_placeholder" : "ic_add"
                BusyIndicator {
                    visible: mainAction.isRunning
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: height
                }

                onClicked: priv.addDevice()
            }
        }
    }*/
    ColumnLayout {

        spacing: 20

        Kirigami.InlineMessage {
            id: message
            type: Kirigami.MessageType.Information
            Layout.fillWidth: true
        }

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

        Controls.Button {

            icon.name: "ic_add"
            text: "Add"
            onClicked: priv.addDevice()
        }

        QtObject {
            id: priv

            function addDevice() {
                message.visible = false
                let future = DeviceManager.addDevice(
                        hostField.text, userField.text, passwordField.text,
                        deviceNameField.text,
                        credentialsDialog.deviceId ? credentialsDialog.deviceId : App.createUuid(
                                                         ))
                Future.sync(future, "isRunning", mainAction)
                Future.onFinished(future, function (result) {
                    console.warn(result.getDetails())
                    if (result.isSuccess()) {
                        credentialsDialog.accepted()
                    } else {
                        message.text = result.getDetails()
                        message.visible = true
                    }
                })
            }
        }
    }


    /*
    footer: DialogButtonBox {

        id: buttonBox
        property bool isRunning: false

        enabled: !isRunning

        Button {
            id: saveButton
            text: qsTr("Save")
            icon.name: "ic_warning"
            onClicked: {
                let future = DeviceManager.addDevice(hostField.text, "", "",
                                                     deviceNameField.text)
                Future.sync(future, "isRunning", buttonBox)
                Future.onFinished(future, function (result) {
                    console.warn("---------- " + Future.result(future))
                    if (!result.ok) {
                        console.warn("error " + result.error)
                    }
                })
            }

            BusyIndicator {
                running: buttonBox.isRunning
                anchors.left: contentItem.left
                anchors.verticalCenter: contentItem
                width: 40
                height: 40
            }
        }
        Button {
            text: qsTr("Close")
            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
        }
    }*/
}
