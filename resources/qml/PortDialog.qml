import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: portDialog
    title: "Port Settings"
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    signal modeSelected(string mode)
    property string selectedMode: configManager.mode
    signal portConfigured(string comPort)
    signal tcpClientConfigured(string ipAddress, string port)
    signal tcpServerConfigured(string port)
    property real dragStartX: 0
    property real dragStartY: 0

    onAccepted: {
        configManager.validateConfiguration()
        modeSelected(selectedMode)
        configManager.mode = selectedMode

        let comPort = comPortCombo.currentText
        portConfigured(comPort)

        if (selectedMode === "TCP Client") {
            tcpClientConfigured(tcpClientIp.text, tcpClientPort.text)
            console.log("TCP Client config:",
                        "IP:", tcpClientIp.text,
                        "Port:", tcpClientPort.text)
        }

        if (selectedMode === "TCP Server") {
            tcpServerConfigured(tcpServerPort.text)
            console.log("TCP Server config:",
                        "Port:", tcpServerPort.text)
        }

        console.log("Accepted with mode:", selectedMode)
        console.log("Accepted with port config:", comPort)
    }

    onRejected: {
        console.log("Canceled")
    }
    Connections {
        target: configManager

        onConfigValidationFailed: (reason) => {
            popup.show(StatusPopup.Type.Error, reason)
            console.log("Validation failed:", reason)
        }
    }

    contentItem: ColumnLayout {
        spacing: 16
        width: 400

        MouseArea {
            id: dragArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            preventStealing: false
            propagateComposedEvents: true

            onPressed: {
                dragStartX = mouse.x
                dragStartY = mouse.y
            }

            onPositionChanged: {
                var newX = portDialog.x + mouse.x - dragStartX
                var newY = portDialog.y + mouse.y - dragStartY

                newX = Math.max(0, Math.min(newX, 1200 - portDialog.width))
                newY = Math.max(0, Math.min(newY, 650 - portDialog.height))

                portDialog.x = newX
                portDialog.y = newY
            }

            onClicked: {
                mouse.accepted = false
            }
        }
        // Chọn chế độ kết nối
        GroupBox {
            title: "Connection Type"
            RowLayout {
                spacing: 8
                RadioButton {
                    text: "Serial"
                    checked: configManager.mode === "Serial"
                    onClicked: configManager.mode = "Serial"

                }
                RadioButton {
                    text: "TCP Client"
                    checked: configManager.mode === "TCP Client"
                    onClicked: configManager.mode = "TCP Client"
                }
                RadioButton {
                    text: "TCP Server"
                    checked: configManager.mode === "TCP Server"
                    onClicked: configManager.mode = "TCP Server"
                }
            }
        }

        // Giao diện Serial
        GroupBox {
            title: "Serial Settings"
            visible: selectedMode === "Serial"
            GridLayout {
                columns: 2
                columnSpacing: 12
                rowSpacing: 8

                Label { text: "COM Port:" }
                ComboBox {
                    id: comPortCombo
                    width: 150
                    model: [
                        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8",
                        "COM9", "COM10", "COM11", "COM12", "COM13", "COM14", "COM15", "COM16",
                        "COM17", "COM18", "COM19", "COM20", "COM21", "COM22", "COM23", "COM24",
                        "COM25", "COM26", "COM27", "COM28", "COM29", "COM30", "COM31", "COM32",
                        "COM33", "COM34", "COM35", "COM36", "COM37", "COM38", "COM39", "COM40",
                        "COM41", "COM42", "COM43", "COM44", "COM45", "COM46", "COM47", "COM48",
                        "COM49", "COM50", "COM51", "COM52", "COM53", "COM54", "COM55", "COM56",
                        "COM57", "COM58", "COM59", "COM60", "COM61", "COM62", "COM63", "COM64"
                    ]
                    currentIndex: model.indexOf(configManager.serialPort)
                    onCurrentTextChanged: configManager.serialPort = currentText
                }


                Label { text: "Baudrate:" }
                ComboBox {
                    model: ["9600", "19200", "38400", "57600", "115200"]
                    width: 150
                    currentIndex: model.indexOf(configManager.serialBaudRate.toString())
                    onCurrentTextChanged: configManager.serialBaudRate = parseInt(currentText)
                }

                Label { text: "Data Bits:" }
                ComboBox {
                    model: ["5", "6", "7", "8"]
                    width: 100
                    currentIndex: model.indexOf(configManager.serialDataBits.toString())
                    onCurrentTextChanged: configManager.serialDataBits = parseInt(currentText)
                }

                Label { text: "Parity:" }
                ComboBox {
                    model: ["None", "Even", "Odd"]
                    width: 100
                    currentIndex: model.indexOf(configManager.serialParity)
                    onCurrentTextChanged: configManager.serialParity = currentText
                }

                Label { text: "Stop Bits:" }
                ComboBox {
                    model: ["1", "2"]
                    width: 100
                    currentIndex: model.indexOf(configManager.serialStopBits.toString())
                    onCurrentTextChanged: configManager.serialStopBits = parseInt(currentText)
                }
            }
        }

        // Giao diện TCP Client
        GroupBox {
            title: "TCP Client Settings"
            visible: selectedMode === "TCP Client"
            GridLayout {
                columns: 2
                columnSpacing: 12
                rowSpacing: 8

                Label { text: "IP Address:" }
                TextField { id: tcpClientIp; text: configManager.tcpClientHost; onTextChanged: configManager.tcpClientHost = text; width: 150 }

                Label { text: "Port:" }

                TextField {
                    id: tcpClientPort
                    text: configManager.tcpClientPort > 0 ? configManager.tcpClientPort.toString() : ""
                    onTextChanged: {
                        const value = parseInt(text)
                        configManager.tcpClientPort = value
                        if (isNaN(value)) {
                            configManager.tcpClientPort = -1
                        }
                    }

                    width: 100
                }
            }
        }

        // Giao diện TCP Server
        GroupBox {
            title: "TCP Server Settings"
            visible: selectedMode === "TCP Server"
            GridLayout {
                columns: 2
                columnSpacing: 12
                rowSpacing: 8

                Label { text: "Port:" }
                TextField {
                    id: tcpServerPort
                    width: 100
                    text: configManager.tcpServerPort > 0 ? configManager.tcpServerPort.toString() : ""
                    onTextChanged: {
                        const value = parseInt(text)
                        console.log("Giá trị nhập vào:", text, "-> parseInt:", value)
                        configManager.tcpServerPort = value
                        if (isNaN(value)) {
                            configManager.tcpServerPort = -1
                        }
                    }
                }
            }
        }
    }
}
