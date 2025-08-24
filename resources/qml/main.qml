import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200 // Chiều dài tổng thể
    height: 650 // Chiều rộng tổng thể, bao gồm 130px cho phần port
    title: "MDISimulator"

    //minimumWidth: 1200
    //maximumWidth: 1200
    //minimumHeight: 650
    //maximumHeight: 650

    TopBar { id: topBar }
    // Phân chia không gian: 520px cho bảng, 130px cho phần port
    Rectangle {
        id: parameterArea
        anchors.fill: parent
        anchors.topMargin: 130
        anchors.bottomMargin: 30
        color: "#36c459ff"

        ParameterTable {
            id: parameterTable
            anchors.fill: parent
            deviceType: "PB"
            command: "MISCF"
        }
    }
    PortDialog {
        id: portDialog
        onModeSelected: {
            topBar.connectionMode = mode
        }
    }
}
