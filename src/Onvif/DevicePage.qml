import QtQuick 2.10
import QtQml 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QuickFuture 1.0
import Onvif
import MaterialRally as Rally

Page {

    id: credentialsDialog
    property string deviceName: ""
    property string deviceEndpoint: ""
    property var deviceId: null
    property bool deviceNameFixed: false
    property bool deviceEndpointFixed: false

    title: qsTr("Edit Devicesss")

    Keys.onEnterPressed: priv.addDevice()
    Keys.onReturnPressed: priv.addDevice()

    signal accepted


    /*
    actions.contextualActions: [

        Kirigami.Action {
            icon.name: "delete"
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
                    icon.name: "content-save"
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
    ]*/
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

        id: form

        //enabled: !mainAction.isRunning
        spacing: 20

        Rally.InlineMessage {
            id: message
            Layout.fillWidth: true
        }

        TextField {

            id: deviceNameField

            text: priv.deviceName
            focus: true
            activeFocusOnTab: true
            Layout.fillWidth: true
            placeholderText: "Device Name"
            selectByMouse: true
            enabled: !credentialsDialog.deviceNameFixed
            validator: RegularExpressionValidator {
                regularExpression: /.+/
            }
        }

        TextField {

            id: hostField

            text: priv.deviceEndpoint
            activeFocusOnTab: true
            Layout.fillWidth: true
            placeholderText: "Device Host"
            selectByMouse: true
            enabled: !credentialsDialog.deviceEndpointFixed
            validator: RegularExpressionValidator {
                regularExpression: /.+/
            }
        }

        TextField {

            id: userField

            text: priv.user
            placeholderText: qsTr("User")
            Layout.fillWidth: true
            activeFocusOnTab: true
            selectByMouse: true
        }

        TextField {

            id: passwordField

            text: priv.password
            echoMode: TextInput.Password
            placeholderText: qsTr("Password")
            Layout.fillWidth: true
            activeFocusOnTab: true
            selectByMouse: true
        }

        BusyCombobox {

            id: defaultMediaProfile

            busy: priv.pendingProfilesRequest
            busyText: qsTr("Searching profiles")
            Layout.fillWidth: true
            model: priv.profiles
            textRole: "name"

            displayText: priv.profiles.length > 0 ? priv.profiles[currentIndex][textRole] : (priv.pendingProfilesRequest ? qsTr("Searching profiles") : qsTr("No profiles found"))
        }

        CameraImage {

            implicitHeight: 200
            implicitWidth: 200

            Layout.alignment: Qt.AlignHCenter

            profileId: Onvif.createProfileId(
                           credentialsDialog.deviceId,
                           priv.profiles[defaultMediaProfile.currentIndex]["profileToken"])
        }

        Row {

            spacing: 20

            Button {

                icon.name: "delete"
                text: "Delete"

                onClicked: priv.deleteDevice()
            }

            Button {

                icon.name: "content-save"
                text: "Save"

                onClicked: priv.addDevice()
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
                        credentialsDialog.deviceId ? credentialsDialog.deviceId : Onvif.createUuid(
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

            function deleteDevice() {

                DeviceManager.removeDevice(credentialsDialog.deviceId)
                credentialsDialog.accepted()
            }
        }
    }
}
