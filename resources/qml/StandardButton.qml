import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control
    width: 200
    height: 30
    font.pixelSize: 12
    padding: 0

    contentItem: Text {
        text: control.text
        font.pixelSize: control.font.pixelSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
    }

    background: Rectangle {
        radius: 6
        color: control.pressed ? "#d0d0d0" : "#e0e0e0"
        border.color: "#999"
        border.width: 1
    }
}
