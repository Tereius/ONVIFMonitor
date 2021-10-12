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

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: pageStack.currentIndex > 0 ? Kirigami.ApplicationHeaderStyle.ShowBackButton : 0
    //pageStack.globalToolBar.minimumHeight: Kirigami.Units.gridUnit * 2
    //pageStack.globalToolBar.preferredHeight: Kirigami.Units.gridUnit * 2
    //pageStack.globalToolBar.maximumHeight: Kirigami.Units.gridUnit * 2
    pageStack.initialPage: Qt.resolvedUrl("MonitoringPage.qml")
    pageStack.interactive: false

    Settings {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }

    Component.onCompleted: {

        DeviceManager.initialize()
    }

    onActiveFocusItemChanged: console.warn("activeFocus: " + activeFocusItem)

    globalDrawer: Kirigami.GlobalDrawer {
        title: Qt.application.name
        titleIcon: "applications-graphics"
        isMenu: !Kirigami.Settings.isMobile
        handleVisible: Kirigami.Settings.isMobile ? pageStack.currentIndex === 0 : false
        enabled: Kirigami.Settings.isMobile ? pageStack.currentIndex === 0 : false
        handleClosedIcon.source: "ic_menu"
        handleOpenIcon.source: "ic_close"

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
                text: qsTr("Monitoring")
                onTriggered: pageStack.replace(Qt.resolvedUrl(
                                                   "MonitoringPage.qml"))
            },
            Kirigami.Action {
                text: qsTr("ONVIF Devices")
                onTriggered: pageStack.replace(Qt.resolvedUrl(
                                                   "DevicesPage.qml"))
            },
            Kirigami.Action {
                text: qsTr("About")
                onTriggered: pageStack.replace(aboutPage)
            }
        ]
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

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

    Component {
        id: aboutPage
        Kirigami.AboutPage {
            aboutData: {
                "displayName": "KirigamiApp",
                "productName": "kirigami/app",
                "componentName": "kirigamiapp",
                "shortDescription": "A Kirigami example",
                "homepage": "",
                "bugAddress": "submit@bugs.kde.org",
                "version": "5.14.80",
                "otherText": "",
                "authors": [{
                                "name": "...",
                                "task": "",
                                "emailAddress": "somebody@kde.org",
                                "webAddress": "",
                                "ocsUsername": ""
                            }],
                "credits": [],
                "translators": [],
                "licenses": [{
                                 "name": "GPL v2",
                                 "text": "long, boring, license text",
                                 "spdx": "GPL-2.0"
                             }],
                "copyrightStatement": "© 2010-2018 Plasma Development Team",
                "desktopFileName": "org.kde.kirigamiapp"
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
