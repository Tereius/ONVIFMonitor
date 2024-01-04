import QtQuick
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts
import QuickFuture
import Onvif
import MaterialRally as Rally

Rally.Dialog {

    id: credentialsDialog

    title: credentialsDialog.deviceId ? qsTr("Edit Device") : qsTr(
                                            "Add new Device")

    onBackButtonClicked: {

        credentialsDialog.close()
    }

    property var deviceId: null
    readonly property string deviceName: ""
    readonly property string deviceEndpoint: ""
    readonly property bool deviceNameFixed: false
    readonly property bool deviceEndpointFixed: false
    readonly property bool isNew: !DeviceManager.containsDevice(
                                      credentialsDialog.deviceId)

    actions: [
        Rally.BusyAction {
            id: deleteAction
            text: qsTr("Delete")
            icon.name: "delete"
            enabled: !credentialsDialog.isNew
            onTriggered: {
                priv.deleteDevice()
            }
        },

        Rally.BusyAction {
            id: addAction
            text: credentialsDialog.isNew ? qsTr("Add") : qsTr("Save")
            icon.name: "check"
            enabled: deviceNameField.acceptableInput
                     && deviceNameField.text.length > 0
                     && hostField.acceptableInput && hostField.text.length > 0
            onTriggered: {
                priv.editDevice()
            }
        }
    ]

    onDeviceIdChanged: {
        priv.initDialog()
    }

    function reload() {
        credentialsDialog.deviceId = credentialsDialog.deviceId
    }

    Rally.ScrollView {

        id: scrollView
        anchors.fill: parent
        Keys.onEnterPressed: priv.editDevice()
        Keys.onReturnPressed: priv.editDevice()
        enableVerticalScrollBar: credentialsDialog.opened
        contentHeight: columnLayout.implicitHeight

        ColumnLayout {

            id: columnLayout
            implicitWidth: Math.min(scrollView.availableWidth, 600)
            anchors.horizontalCenter: parent.horizontalCenter

            Item {

                Layout.fillWidth: true
                implicitHeight: message.count === 0 ? 0 : message.implicitHeight

                Behavior on implicitHeight {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.OutCubic
                    }
                }

                Rally.InlineMessage {
                    id: message
                    title: qsTr("Error")
                    width: parent.width

                    Behavior on implicitHeight {
                        enabled: false
                    }
                }
            }

            Rally.GroupBox {

                title: qsTr("Basic settings")
                icon.name: "cog"
                Layout.fillWidth: true

                Rally.FormLayout {

                    width: parent.width

                    Label {
                        text: qsTr("Device name")
                    }

                    Rally.TextField {

                        id: deviceNameField
                        text: credentialsDialog.deviceName
                        focus: true
                        enabled: !credentialsDialog.deviceNameFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    Label {
                        text: qsTr("Device endpoint")
                    }

                    Rally.TextField {

                        id: hostField
                        text: credentialsDialog.deviceEndpoint
                        enabled: !credentialsDialog.deviceEndpointFixed
                        validator: RegularExpressionValidator {
                            regularExpression: /.+/
                        }
                    }

                    Label {
                        text: qsTr("Credentials")
                    }

                    Rally.TextField {

                        id: userField
                        placeholderText: qsTr("User")
                        font.family: "Roboto Mono"
                    }

                    Rally.PasswordTextField {

                        id: passwordField
                        placeholderText: qsTr("Password")
                        font.family: "Roboto Mono"
                    }
                }
            }
        }
    }

    QtObject {
        id: priv

        function initDialog() {

            if (credentialsDialog.deviceId && DeviceManager.containsDevice(
                        credentialsDialog.deviceId)) {
                const daviceInfo = DeviceManager.getDeviceInfo(
                                     credentialsDialog.deviceId)
                deviceNameField.text = DeviceManager.getName(
                            credentialsDialog.deviceId)
                hostField.text = daviceInfo.deviceEndpoint
                userField.text = daviceInfo.user
                passwordField.text = daviceInfo.password

                if (!daviceInfo.initialized) {
                    message.pushMessage(daviceInfo.initializationError,
                                        "error", qsTr("Initialization error"))
                }
            }
        }

        function editDevice() {

            if (credentialsDialog.isNew) {

                // Add device case
                credentialsDialog.busy = true
                let future = DeviceManager.addDevice(
                        hostField.text, userField.text, passwordField.text,
                        deviceNameField.text,
                        credentialsDialog.deviceId ? credentialsDialog.deviceId : Onvif.createUuid(
                                                         ))
                Future.onFinished(future, function (result) {

                    if (!result.isSuccess()) {
                        message.pushMessage(result.getDetails(), "error",
                                            qsTr("Initialization error"))
                    } else {
                        credentialsDialog.reload()
                    }
                    credentialsDialog.busy = false
                }, function () {
                    credentialsDialog.busy = false
                })
            } else {

                // Edit device case
                credentialsDialog.busy = true
                DeviceManager.renameDevice(credentialsDialog.deviceId,
                                           deviceNameField.text)

                let future = DeviceManager.setDeviceCredentials(
                        credentialsDialog.deviceId, userField.text,
                        passwordField.text)

                Future.onFinished(future, function (result) {

                    if (!result.isSuccess()) {
                        message.pushMessage(result.getDetails(), "error",
                                            qsTr("Initialization error"))
                    } else {
                        message.clear()
                    }
                    credentialsDialog.reload()
                    credentialsDialog.busy = false
                }, function () {
                    credentialsDialog.busy = false
                })
            }
        }

        function deleteDevice() {

            if (credentialsDialog.deviceId) {
                DeviceManager.removeDevice(credentialsDialog.deviceId)
                credentialsDialog.close()
            }
        }
    }
}
