import QtQuick
import QtMultimedia
import QtQuick.Controls

ComboBox {

    MediaDevices {
        id: devices
    }

    model: {
        let deviceList = []
        const defaultDevice = devices.defaultAudioInput
        if (defaultDevice.mode === AudioDevice.Input) {
            deviceList.push({
                                "description": qsTr("System default audio input"),
                                "value": defaultDevice
                            })
        }
        for (var i = 0; i < devices.audioInputs.length; i++) {
            if (devices.audioInputs[i].mode === AudioDevice.Input) {
                deviceList.push({
                                    "description": devices.audioInputs[i].description
                                    + (devices.audioInputs[i].isDefault ? " *" : ""),
                                    "value": devices.audioInputs[i]
                                })
            }
        }
        return deviceList
    }
    textRole: "description"
    valueRole: "value"
}
