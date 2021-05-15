import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.onvif.device 1.0

Dialog {

    id: busyPopup

    x: Math.round((window.width - width) / 2)
    y: Math.round((window.height - 2 * window.header.height - height) / 2)
    width: 100
    height: 100
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose
    z: 99

    //enter: undefined
    //exit: undefined
    contentItem: BusyIndicator {

        anchors.centerIn: parent
        running: true
    }
}
