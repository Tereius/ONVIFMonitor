import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.16 as Kirigami

Kirigami.OverlayDrawer {

    id: credentialsDialog
    property alias user: userTextField.text
    property alias pwd: pwdTextField.text
    property alias save: saveCheckBox.checked
    property url endpoint
    property var deviceId

    edge: Qt.BottomEdge

    signal accepted
    signal rejected

    onAboutToShow: {

        userTextField.clear()
        pwdTextField.clear()
        save = false
    }

    contentItem: GridLayout {

        columns: 2

        Label {
            text: qsTr("User")
        }
        TextField {

            id: userTextField

            activeFocusOnTab: true
            placeholderText: "Name"
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Password")
        }
        TextField {

            id: pwdTextField

            echoMode: TextInput.PasswordEchoOnEdit

            activeFocusOnTab: true
            placeholderText: "*********"
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Remember")
        }
        CheckBox {

            id: saveCheckBox

            checked: false
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Save the credentials")
        }

        DialogButtonBox {

            Layout.columnSpan: 2
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: {
                credentialsDialog.close()
                credentialsDialog.accepted()
            }

            onRejected: {
                credentialsDialog.close()
                credentialsDialog.rejected()
            }
        }
    }
}
