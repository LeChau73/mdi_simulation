import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: exitBar
    height: 40
    color: "#b91818ff"
    border.color: "#ccc"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 10

        Item { Layout.fillWidth: true }

        StandardButton {
            text: "Exit"
            onClicked: Qt.quit()
        }
    }
}
