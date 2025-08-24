import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform 1.1
import "."

Rectangle {
    id: topBar
    property string connectionMode: configManager.mode
    property int shortFieldWidth: 60
    property int mediumFieldWidth: 120
    property int longFieldWidth: 300

    property string currentComPort: configManager.serialPort
    property string tcpClientIpAddress: configManager.tcpClientHost
    property string tcpClientPort: configManager.tcpClientPort.toString()
    property string tcpServerPort: configManager.tcpServerPort.toString()

    Connections {
        target: configManager

        onConfigLoadSuccess: (message) => {
            popup.show(StatusPopup.Type.Success, message)
        }

        onConfigLoadFailed: (reason) => {
            popup.show(StatusPopup.Type.Error, reason)
        }


        onConfigSaveSuccess: (message) => {
            popup.show(StatusPopup.Type.Success, message)
        }

        onConfigSaveFailed: (reason) => {
            popup.show(StatusPopup.Type.Error, reason)
        }

        onConnectionOpenSuccess: (message) => {
            popup.show(StatusPopup.Type.Success, message)
        }

        onConnectionOpenFailed: (reason) => {
            popup.show(StatusPopup.Type.Error, reason)
        }

        onConnectionCloseSuccess: (message) => {
            popup.show(StatusPopup.Type.Success, message)
        }

        onConnectionCloseFailed: (reason) => {
            popup.show(StatusPopup.Type.Error, reason)
        }
    }

    PortDialog {
        id: portDialog

        onPortConfigured: {
            currentComPort = comPort
        }

        onModeSelected: {
            connectionMode = mode
        }

        onTcpClientConfigured: {
            configManager.tcpClientHost = ipAddress
            configManager.tcpClientPort = port
            console.log("TCP Client configured:")
            console.log("IP Address:", ipAddress)
            console.log("Port:", port)
        }

        onTcpServerConfigured: {
            configManager.tcpServerPort = port
            console.log("TCP Server configured:")
            console.log("Port:", port)
        }
    }

    height: 10
    width: parent.width
    color: "#f5f5f5"
    border.color: "#ccc"
    anchors.top: parent.top

    FileDialog {
        id: saveAsDialog
        title: "Save As"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Config Files (*.config)", "All Files (*)"]
        onAccepted: {
            configManager.saveToFile(saveAsDialog.currentFile)
            console.log("File saved to:", saveAsDialog.file)
        }
        onRejected: {
            console.log("Save As dialog canceled")
        }
    }

    FileDialog {
        id: loadConfigDialog
        title: "Open Config File"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Config Files (*.config)", "All Files (*)"]
        onAccepted: {
            var localPath = loadConfigDialog.file.toString().replace("file:///", "")
            configManager.loadFromFile(localPath)
            configPath.text = localPath
        }

        onRejected: {
            console.log("Open dialog canceled")
        }
    }

    //Infor: Load file CSV
    FileDialog {
        id: loadCsvDialog
        title: "Open Data File"
        fileMode: FileDialog.OpenFile
        onAccepted: {
            var localPath = loadCsvDialog.file.toString().replace("file:///", "")
            mainWindowController.loadFileCsvFrom(localPath)
            csvPath.text = localPath
        }

        onRejected: {
            console.log("Open dialog canceled")
        }
    }

    FileDialog {
        id: saveCsvDialog
        title: "Open Config File"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Config Files (*.config)", "All Files (*)"]
        onAccepted: {
            var localPath = loadConfigDialog.file.toString().replace("file:///", "")
            configManager.loadFromFile(localPath)
            configPath.text = localPath
        }

        onRejected: {
            console.log("Open dialog canceled")
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 20

        ColumnLayout {
            Layout.preferredWidth: 600
            Loader {
                id: modeLoader
                sourceComponent: {
                    switch (topBar.connectionMode) {
                        case "Serial": return serialComponent
                        case "TCP Client": return tcpClientComponent
                        case "TCP Server": return tcpServerComponent
                        default: return null
                    }
                }
            }
            // === Các Component cho từng mode ===
            Component {
                id: serialComponent
                RowLayout {
                    spacing: 20

                    GroupBox {
                        title: "RS232C"
                        Layout.preferredWidth : 510
                        Layout.preferredHeight : 70
                        RowLayout {
                            spacing: 8
                            Label { text: "Protocol" }
                            TextField { text: "PB"; readOnly: true; Layout.preferredWidth: mediumFieldWidth }
                            Label { text: "COM" }
                            TextField { text: currentComPort ; readOnly: true; Layout.preferredWidth: shortFieldWidth }
                            Item { Layout.fillHeight: true }
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.topMargin : 10
                        Layout.leftMargin: -10
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        StandardButton { text: "Open"; Layout.preferredWidth: 70 }
                        StandardButton { text: "Closed"; Layout.preferredWidth: 70 }
                    }
                }
            }

            Component {
                id: tcpClientComponent
                RowLayout {
                    spacing: 20

                    GroupBox {
                        title: "TCP Client"
                        Layout.preferredWidth : 510
                        Layout.preferredHeight : 70
                        RowLayout {
                            spacing: 8
                            Label { text: "Protocol" }
                            TextField { text: "PB"; readOnly: true; Layout.preferredWidth: mediumFieldWidth }
                            Label { id: ipTcpClient; text: "IP Address" }
                            TextField { text: tcpClientIpAddress; readOnly: true; Layout.preferredWidth: mediumFieldWidth }
                            Label { id: portTcpClient; text: "Port" }
                            TextField { text: tcpClientPort; readOnly: true; Layout.preferredWidth: shortFieldWidth }
                            Item { Layout.fillHeight: true }
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.topMargin : 10
                        Layout.leftMargin: -10
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                        StandardButton {
                            text: "Open"
                            enabled: !tcpClient.isConnected
                            Layout.preferredWidth: 70
                            onClicked: {
                                console.log("🔍 IP:", tcpClientIpAddress, "Port:", parseInt(tcpClientPort))
                                tcpClient.connectToHost(tcpClientIpAddress, parseInt(tcpClientPort))
                            }
                        }

                        StandardButton {
                            text: "Closed"
                            enabled: tcpClient.isConnected
                            Layout.preferredWidth: 70
                            onClicked: {
                                tcpClient.disconnectFromHost()
                            }
                        }

                    }
                }
            }

            Component {
                id: tcpServerComponent
                ColumnLayout {
                    spacing: 10

                    RowLayout {
                        spacing: 20

                        GroupBox {
                            title: "TCP Server"
                            Layout.preferredWidth : 510
                            Layout.preferredHeight : 70
                            RowLayout {
                                spacing: 8
                                Label { text: "Protocol" }
                                TextField { text: "PB"; readOnly: true; Layout.preferredWidth: mediumFieldWidth }
                                Label { text: "Port" }
                                TextField { text: tcpServerPort; readOnly: true; Layout.preferredWidth: shortFieldWidth }
                                Item { Layout.fillHeight: true }
                            }
                        }

                        ColumnLayout {
                            spacing: 4
                            Layout.topMargin : 10
                            Layout.leftMargin: -10
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                            StandardButton {
                                text: "Open"
                                enabled: !tcpServer.isListening
                                Layout.preferredWidth: 70
                                onClicked: {
                                    const port = parseInt(tcpServerPort)
                                    console.log( "--->Port server:", parseInt(tcpServerPort))
                                    tcpServer.startServer(port)
                                }
                            }

                            StandardButton {
                                text: "Closed"
                                enabled: tcpServer.isListening
                                Layout.preferredWidth: 70
                                onClicked: {
                                    tcpServer.stopServer()
                                    console.log("TCP Server stopped")
                                }
                            }
                        }
                    }
                }
            }


            // === File Config Shared ===
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Label { text: "File path:" }
                TextField {
                    id: configPath
                    text: configManager.configPath
                    Layout.preferredWidth: longFieldWidth
                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredHeight: 25
                }
                StandardButton {
                    text: "Load config"
                    Layout.preferredWidth: 70
                    onClicked: {
                        loadConfigDialog.open()
                    }
                }

                StandardButton {
                    text: "Save As"
                    Layout.preferredWidth: 70
                    onClicked: {
                        saveAsDialog.open()
                    }
                }
                StandardButton {
                    text: "Setting port"
                    Layout.preferredWidth: 70
                    onClicked: {
                        portDialog.x = (1200 - portDialog.width) / 2
                        portDialog.y = (650 - portDialog.height) / 2
                        portDialog.open()
                    }
                }
            }
        }

        Rectangle { width: 1; height: 100; color: "#aaa" }

        // Send/Receive group
        GridLayout {
            columns: 3
            columnSpacing: 10
            rowSpacing: 4
            Layout.fillWidth: true

            // Row 1: Received Data
            Label { text: "Received Data" }
            TextField {
                id: receivedData
                Layout.preferredWidth: longFieldWidth
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: 25
                placeholderText: "..."
            }
            Item {}
            // Row 2: Send Data
            Label { text: "Send Data" }
            TextField {
                id: sendData
                Layout.preferredWidth: longFieldWidth
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: 25
                placeholderText: "..."
            }
            StandardButton { text: "Send data"; Layout.preferredWidth: 70; Layout.alignment: Qt.AlignRight}

            // Row 3: File Path
            Label { text: "File path:" }
            TextField {
                id: csvPath
                text: "C:\\MDI_SIMDI_PB\\default.csv"
                Layout.preferredWidth: longFieldWidth
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: 25
            }
            RowLayout {
                spacing: 4

                StandardButton {
                    text: "Load file"
                    Layout.preferredWidth: 70
                    onClicked: {
                        loadCsvDialog.title = "Open Data File"
                        loadCsvDialog.nameFilters = ["Text Files (*.csv)", "All Files (*)"]
                        loadCsvDialog.open()
                    }

                }

                StandardButton {
                    text: "Save file"
                    Layout.preferredWidth: 70
                    onClicked: {
                        saveCsvDialog.title = "Save Data File"
                        saveCsvDialog.nameFilters = ["Config Files (*.csv)", "All Files (*)"]
                        saveCsvDialog.fileMode = FileDialog.SaveFile
                        saveCsvDialog.open()
                    }
                }

            }
        }

        Item { Layout.fillWidth: true }
        RowLayout {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 6
            Rectangle {
                width: 16; height: 16; radius: 8
                color: {
                    console.log("connectionMode:", connectionMode)
                    if (connectionMode === "TCP Client")
                        return tcpClient.isConnected ? "green" : "red"
                    else if (connectionMode === "TCP Server")
                        return tcpServer.isListening ? "green" : "red"
                    else
                        return serial.isConnected ? "green" : "red"
                }
            }

            Label {
                text: {
                    if (connectionMode === "TCP Client")
                        return tcpClient.isConnected ? "Connected" : "Disconnected"
                    else if (connectionMode === "TCP Server")
                        return tcpServer.isListening ? "Connected" : "Disconnected"
                    else
                        return serial.isConnected ? "Connected" : "Disconnected"
                }
            }
        }
    }
}
