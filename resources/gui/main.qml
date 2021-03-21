import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.settings 1.0
import org.onvif.device 1.0
import org.onvif.event 1.0
import org.global 1.0
import org.kde.kirigami 2.16 as Kirigami

Kirigami.ApplicationWindow {
    id: root

    title: Qt.application.name

    globalDrawer: Kirigami.GlobalDrawer {
        title: Qt.application.name
        titleIcon: "applications-graphics"
        actions: [
            Kirigami.Action {
                text: qsTr("View")
                icon.name: "view-list-icons"
                Kirigami.Action {
                    text: qsTr("View Action 1")
                    onTriggered: showPassiveNotification(
                                     qsTr("View Action 1 clicked"))
                }
                Kirigami.Action {
                    text: qsTr("View Action 2")
                    onTriggered: showPassiveNotification(
                                     qsTr("View Action 2 clicked"))
                }
            },
            Kirigami.Action {
                text: qsTr("ONVIF Devices")
                onTriggered: pageStack.replace(Qt.resolvedUrl(
                                                   "DeviceSettingsPage.qml"))
            },
            Kirigami.Action {
                text: qsTr("Action 2")
                onTriggered: showPassiveNotification(qsTr("Action 2 clicked"))
            }
        ]
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: mainPageComponent

    Component {
        id: mainPageComponent

        Kirigami.Page {

            title: Qt.application.name

            actions {
                main: Kirigami.Action {
                    icon.name: "go-home"
                    onTriggered: sheet.open()
                }
                left: Kirigami.Action {
                    icon.name: "go-previous"
                    onTriggered: showPassiveNotification(
                                     qsTr("Left action triggered"))
                }
                right: Kirigami.Action {
                    icon.name: "go-next"
                    onTriggered: showPassiveNotification(
                                     qsTr("Right action triggered"))
                }
                contextualActions: [
                    Kirigami.Action {
                        text: qsTr("Contextual Action 1")
                        icon.name: "bookmarks"
                        onTriggered: showPassiveNotification(
                                         qsTr("Contextual action 1 clicked"))
                    },
                    Kirigami.Action {
                        text: qsTr("Contextual Action 2")
                        icon.name: "folder"
                        enabled: false
                    }
                ]
            }

            Kirigami.OverlaySheet {
                id: sheet
                onSheetOpenChanged: page.actions.main.checked = sheetOpen
                Label {
                    wrapMode: Text.WordWrap
                    text: "Lorem ipsum dolor sit amet"
                }
            }
        }
    }

    CredentialsDialog {

        id: credentialsDialog

        Connections {

            target: DeviceManager
            onUnauthorized: {

                credentialsDialog.deviceId = rDeviceId
                credentialsDialog.open()
            }
        }


        /* onAccepted: {

            DeviceManager.setDeviceCredentials(credentialsDialog.deviceId,
                                               credentialsDialog.user,
                                               credentialsDialog.pwd,
                                               credentialsDialog.save)
            window.credentialsProvided(credentialsDialog.deviceId)
        }*/
    }
}
