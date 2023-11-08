import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 2.5
import "controls" as Controls

Controls.ComboBox {

    id: component
    property bool busy: false
    property string busyText: ""
    property string emptyText: ""

    enabled: !busy && count > 0

    indicator.visible: !busy
    displayText: busy ? busyText : (count == 0 ? emptyText : (textRole ? ((Array.isArray(model) ? modelData[textRole] : model[textRole])) : modelData))

    BusyIndicator {
        id: busyIndicator
        visible: component.busy
        running: visible
        anchors.centerIn: indicator
        height: parent.height
        width: height
    }
}
