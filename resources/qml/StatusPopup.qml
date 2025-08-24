import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: statusPopup
    width: 300
    height: 150
    modal: true
    focus: true
    clip: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    enum Type { Info, Success, Error, Warning }

    property string titleText: "Error"
    property string messageText: "Cannot connect to device!"
    property url iconSource: "qrc:/app/icons/error.png"
    property color backgroundColor: "#ffffff"
    property color titleColor: "#D32F2F"

    Rectangle {
        anchors.fill: parent
        color: backgroundColor
        border.color: "#ccc"
        radius: 4

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 0
            spacing: 0

            // === Title ===
            Rectangle {
                Layout.fillWidth: true
                height: 36
                color: "transparent"

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: titleText
                    font.bold: true
                    font.pointSize: 13
                    color: titleColor
                }
            }

            // === Horizontal line ===
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#ccc"
            }

            // === Content ===
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.margins: 12
                spacing: 8

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 8

                    Image {
                        source: iconSource
                        width: 28
                        height: 28
                        fillMode: Image.PreserveAspectFit
                    }

                    Item {
                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 65
                        clip: true

                        MouseArea {
                            id: messageArea
                            anchors.fill: parent
                            hoverEnabled: true

                            ToolTip.visible: containsMouse
                            ToolTip.text: messageText

                            Text {
                                anchors.fill: parent
                                text: messageText
                                wrapMode: Text.WordWrap
                                elide: Text.ElideNone
                                color: "#333"
                                font.pointSize: 12
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignTop
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    width: 50
                    height: 36

                    Button {
                        width: 50
                        text: "OK"
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: statusPopup.close()
                    }
                }
            }
        }
    }

    function show(type, message) {
        messageText = message
        setType(type)
        statusPopup.open()
    }

    function setType(type) {
        switch (type) {
        case StatusPopup.Type.Error:
            titleText = "Error"
            iconSource = "qrc:/app/icons/error.png"
            backgroundColor = "#ffffff"
            titleColor = "#D32F2F"
            break
        case StatusPopup.Type.Success:
            titleText = "Success"
            iconSource = "qrc:/app/icons/success.png"
            backgroundColor = "#ffffff"
            titleColor = "#388E3C"
            break
        }
    }
}
