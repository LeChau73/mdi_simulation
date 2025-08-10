import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200 // Chiều dài tổng thể
    height: 650 // Chiều rộng tổng thể, bao gồm 130px cho phần port
    title: "MDISimulator"

    // Phân chia không gian: 520px cho bảng, 130px cho phần port
    Rectangle {
        id: parameterArea
        width: 1200
        height: 620
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 15
        color: "#f0f0f0"

        ParameterTable {
            id: parameterTable
            anchors.fill: parent
            deviceType: "PB"
            command: "MISCF"
        }
    }
}
