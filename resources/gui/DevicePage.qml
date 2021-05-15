import QtQuick 2.10
import QtQml 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QuickFuture 1.0
import org.onvif.device 1.0
import org.kde.kirigami 2.14 as Kirigami
import "helper.js" as Helper

Kirigami.ScrollablePage {

    id: credentialsDialog
    property string deviceName: ""
    property string deviceEndpoint: ""
    property var deviceId: null
    property bool deviceNameFixed: false
    property bool deviceEndpointFixed: false

    title: qsTr("Edit Device")

    Keys.onEnterPressed: priv.addDevice()
    Keys.onReturnPressed: priv.addDevice()

    actions.contextualActions: [

        Kirigami.Action {
            icon.name: "ic_delete"
            text: qsTr("Delete")
            onTriggered: priv.deleteDevice()
        },

        Kirigami.Action {

            id: mainAction
            property bool isRunning: false

            displayComponent: Component {
                ToolButton {
                    text: qsTr("Save")
                    enabled: !mainAction.isRunning
                             && deviceNameField.acceptableInput
                             && hostField.acceptableInput
                    icon.name: mainAction.isRunning ? "ic_placeholder" : "ic_save"
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
        }
    ]

    onDeviceIdChanged: {

        message.visible = false
        var deviceInfo = DeviceManager.getDeviceInfo(credentialsDialog.deviceId)
        priv.deviceName = DeviceManager.getName(credentialsDialog.deviceId)
        priv.deviceEndpoint = deviceInfo.deviceEndpoint
        priv.user = deviceInfo.user
        priv.password = deviceInfo.password
        if (deviceInfo.initialized) {
            priv.profiles = []
            priv.pendingProfilesRequest = true
            var future = DeviceManager.getMediaProfiles(
                        credentialsDialog.deviceId)
            Future.onFinished(future, function (result) {
                if (result.isSuccess()) {
                    priv.profiles = Future.result(future)
                }
                priv.pendingProfilesRequest = false
            })
        } else {
            message.text = deviceInfo.initializationError
            message.visible = true
        }
    }

    ColumnLayout {

        Kirigami.InlineMessage {
            id: message
            Layout.fillWidth: true
            type: Kirigami.MessageType.Information
        }

        FormLayout {

            id: form
            enabled: !mainAction.isRunning

            TextField {

                id: deviceNameField

                Component.onCompleted: {
                    ensureVisible(0)
                }

                Kirigami.FormData.label: Helper.markRejectedLabel(
                                             qsTr("Name"), acceptableInput)

                text: priv.deviceName
                focus: true
                activeFocusOnTab: true
                placeholderText: "Device Name"
                selectByMouse: true
                enabled: !credentialsDialog.deviceNameFixed
                validator: RegExpValidator {
                    regExp: /.+/
                }
            }

            TextField {

                id: hostField

                Component.onCompleted: {
                    ensureVisible(0)
                }

                Kirigami.FormData.label: Helper.markRejectedLabel(
                                             qsTr("Host"), acceptableInput)

                text: priv.deviceEndpoint
                activeFocusOnTab: true
                placeholderText: "Device Host"
                selectByMouse: true
                enabled: !credentialsDialog.deviceEndpointFixed
                validator: RegExpValidator {
                    regExp: /.+/
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.label: qsTr("Credentials")
                Kirigami.FormData.isSection: true
            }

            TextField {

                id: userField

                Kirigami.FormData.label: qsTr("User")

                text: priv.user
                placeholderText: qsTr("User")
                activeFocusOnTab: true
                selectByMouse: true
            }

            Kirigami.PasswordField {

                id: passwordField

                Kirigami.FormData.label: qsTr("Password")

                text: priv.password
                passwordCharacter: "\u2022"
                placeholderText: qsTr("Password")
                activeFocusOnTab: true
                selectByMouse: true
            }

            Kirigami.Separator {
                Kirigami.FormData.label: qsTr("Media")
                Kirigami.FormData.isSection: true
            }

            BusyCombobox {

                id: defaultMediaProfile

                Kirigami.FormData.label: qsTr("Default Profile")

                Layout.fillWidth: true

                busy: priv.pendingProfilesRequest
                busyText: qsTr("Searching profiles")
                model: priv.profiles
                textRole: "name"

                displayText: priv.profiles.length > 0 ? priv.profiles[currentIndex][textRole] : (priv.pendingProfilesRequest ? qsTr("Searching profiles") : qsTr("No profiles found"))
            }

            CameraImage {

                Layout.fillWidth: true
                implicitHeight: 300
                profileId: App.createProfileId(
                               credentialsDialog.deviceId,
                               priv.profiles[defaultMediaProfile.currentIndex]["profileToken"])
            }
        }
    }

    QtObject {
        id: priv

        property string deviceName: ""
        property string deviceEndpoint: ""
        property string user: ""
        property string password: ""
        property bool pendingProfilesRequest: false
        property var profiles: []

        onPendingProfilesRequestChanged: {
            console.warn("############### " + pendingProfilesRequest)
        }

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
                    pageStack.pop()
                } else {
                    message.text = result.getDetails()
                    message.visible = true
                }
            })
        }

        function deleteDevice() {

            DeviceManager.removeDevice(credentialsDialog.deviceId)
            pageStack.pop()
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
