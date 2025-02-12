import QtQuick
import QtQml
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.settings
import QtQuick.Controls.Material
import MaterialRally as Rally

Rally.RallyApplicationWindow {

    id: root

    ListModel {

        id: mainMenuModel

        ListElement {
            title: qsTr("Monitoring")
            iconName: "cctv"
            defaultChecked: true
            qmlSource: "MonitoringPage.qml"
        }

        ListElement {
            title: qsTr("Recording")
            iconName: "harddisk"
            qmlSource: "PlaceholderPage.qml"
        }

        ListElement {
            title: qsTr("Settings")
            iconName: "cog"
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

    header: Rally.ToolBar {

        Rally.TabBarFolding {

            id: tabBar
            anchors.centerIn: parent
            width: Math.min(parent.width, 500)

            onIndexSelected: index => view.currentIndex = index
            currentIndex: view.currentIndex

            Repeater {
                model: mainMenuModel
                Rally.TabButtonFolding {

                    checked: defaultChecked
                    icon.name: iconName
                    text: title
                }
            }
        }

        ProgressBar {
            anchors.top: parent.bottom
            width: parent.width
            indeterminate: true

            visible: false //loader.status != Loader.Ready

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
    }

    Rally.ScrollView {

        id: scrollView
        anchors.fill: parent

        SwipeView {

            id: view
            width: parent.width
            implicitHeight: Math.max(scrollView.availableHeight, currentItem.implicitHeight)

            Repeater {
                model: mainMenuModel
                Loader {
                    id: loader
                    active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                    asynchronous: true
                    source: Qt.resolvedUrl(qmlSource)
                    visible: status == Loader.Ready
                }
            }
        }
    }
}
