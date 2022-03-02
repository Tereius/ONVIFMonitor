import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0
import QtQuick.Controls.Material 2.12
import "controls" as Controls

ApplicationWindow {

    id: root
    visible: true

    minimumWidth: 290
    minimumHeight: 290 + root.header.height

    Settings {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }

    Label {

        id: sizeLabel
        padding: 10
        text: "w: " + root.width + " h: " + root.height
        anchors.centerIn: root.contentItem
        z: 1

        Connections {
            target: root
            onWidthChanged: {
                sizeLabel.visible = true
                sizeLabelTimer.restart()
            }
        }

        Timer {
            id: sizeLabelTimer
            interval: 1000
            onTriggered: {
                parent.visible = false
            }
        }

        background: Rectangle {
            color: "black"
            opacity: 0.5
            radius: 4
        }
    }

    ListModel {

        id: mainMenuModel

        ListElement {
            title: qsTr("Overview")
            iconName: "cctv"
            defaultChecked: true
            qmlSource: "MonitoringPage.qml"
        }

        ListElement {
            title: qsTr("Recordings")
            iconName: "harddisk"
            qmlSource: "PlaceholderPage.qml"
        }

        ListElement {
            title: qsTr("Settings")
            iconName: "ic_settings"
            qmlSource: "DevicesPage.qml"
        }

        ListElement {
            title: qsTr("About")
            iconName: "information"
            qmlSource: "AboutPage.qml"
        }

        ListElement {
            title: qsTr("Style")
            iconName: "information"
            qmlSource: "Style.qml"
        }
    }

    header: Controls.ToolBar {

        Controls.TabBarFolding {

            id: tabBar
            anchors.centerIn: parent
            width: Math.min(parent.width, 500)

            onIndexSelected: index => view.currentIndex = index
            currentIndex: view.currentIndex

            Repeater {
                model: mainMenuModel
                Controls.TabButtonFolding {

                    checked: defaultChecked
                    icon.name: iconName
                    text: title
                }
            }
        }
    }

    ProgressBar {
        anchors.top: root.header
        width: root.header.width
        indeterminate: true

        visible: loader.status === Loader.Loading

        Material.accent: Material.iconColor

        Component.onCompleted: {
            contentItem.implicitHeight = 2
        }

        background: Rectangle {
            implicitHeight: 2
            color: Material.iconColor
            opacity: 0.6
        }
    }

    SwipeView {

        id: view
        anchors.fill: parent

        Repeater {
            model: mainMenuModel
            Loader {
                id: loader
                active: SwipeView.isCurrentItem || SwipeView.isNextItem
                        || SwipeView.isPreviousItem
                asynchronous: true
                source: Qt.resolvedUrl(qmlSource)
                visible: status == Loader.Ready
            }
        }
    }
}
