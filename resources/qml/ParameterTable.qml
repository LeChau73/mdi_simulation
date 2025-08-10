import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: parameterTable
    anchors.fill: parent

    property string deviceType: "PB"
    property string command: "MISCF"
    property var tableModel: null
    
    // Tính toán layout
    property int itemHeight: 35  // Chiều cao mỗi hàng parameter
    property int headerHeight: 30 // Chiều cao header
    property int nameWidth: 120   // Chiều rộng cột Name
    property int valueWidth: 120  // Chiều rộng cột Value  
    property int sendWidth: 60    // Chiều rộng cột Send
    property int totalItemWidth: nameWidth + valueWidth + sendWidth // Tổng chiều rộng 1 nhóm
    property int spacing: 10      // Khoảng cách giữa các cột
    
    // Tính số hàng có thể hiển thị trong chiều cao available
    property int availableHeight: parent.height - 103 - headerHeight - 40 // Trừ margin top, header và margin bottom
    property int maxRowsPerColumn: Math.floor(availableHeight / itemHeight)
    
    // Tính số cột cần thiết
    property int totalItems: tableModel ? tableModel.rowCount() : 0
    property int numberOfColumns: Math.ceil(totalItems / maxRowsPerColumn)

    // Components cho các loại input
    Component {
        id: textFieldComponent
        TextField {
            property int itemIndex: parent.parent.itemIndex
            text: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let valueIndex = tableModel.index(itemIndex, 1)
                    return tableModel.data(valueIndex, Qt.DisplayRole) || ""
                }
                return ""
            }
            onTextChanged: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let modelIndex = tableModel.index(itemIndex, 1)
                    tableModel.setData(modelIndex, text, Qt.EditRole)
                    
                    let nameIndex = tableModel.index(itemIndex, 0)
                    let sendIndex = tableModel.index(itemIndex, 2)
                    let rawName = tableModel.data(nameIndex, Qt.DisplayRole) || ""
                    let paramName = rawName.replace(/\s*\(.*\)\s*$/, "") // Xóa phần "(xxbyte)"
                    let send = tableModel.data(sendIndex, Qt.DisplayRole) === "1"
                    
                    mainWindowController.updateCsvValue(deviceType, command, paramName, text, send)
                }
            }
            width: parent.width - 4
            height: parent.height - 4
            anchors.centerIn: parent
            horizontalAlignment: TextInput.AlignHCenter
        }
    }
    
    Component {
        id: optionComponent
        ComboBox {
            property int itemIndex: parent.parent.itemIndex
            property var optionsList: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let nameIndex = tableModel.index(itemIndex, 0)
                    return tableModel.data(nameIndex, Qt.UserRole + 2) || []
                }
                return []
            }
            model: optionsList
            currentIndex: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let valueIndex = tableModel.index(itemIndex, 1)
                    let val = tableModel.data(valueIndex, Qt.DisplayRole) || ""
                    return optionsList.indexOf(val)
                }
                return -1
            }
            onCurrentTextChanged: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let modelIndex = tableModel.index(itemIndex, 1)
                    tableModel.setData(modelIndex, currentText, Qt.EditRole)
                    
                    let nameIndex = tableModel.index(itemIndex, 0)
                    let sendIndex = tableModel.index(itemIndex, 2)
                    let rawName = tableModel.data(nameIndex, Qt.DisplayRole) || ""
                    let paramName = rawName.replace(/\s*\(.*\)\s*$/, "") // Xóa phần "(xxbyte)"
                    let send = tableModel.data(sendIndex, Qt.DisplayRole) === "1"
                    
                    mainWindowController.updateCsvValue(deviceType, command, paramName, currentText, send)
                }
            }
            width: parent.width - 4
            height: parent.height - 4
            anchors.centerIn: parent
        }
    }
    
    Component {
        id: stateComponent
        ComboBox {
            property int itemIndex: parent.parent.itemIndex
            property var statesList: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let nameIndex = tableModel.index(itemIndex, 0)
                    return tableModel.data(nameIndex, Qt.UserRole + 2) || []
                }
                return []
            }
            model: statesList
            currentIndex: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let valueIndex = tableModel.index(itemIndex, 1)
                    let val = tableModel.data(valueIndex, Qt.DisplayRole) || ""
                    return statesList.indexOf(val)
                }
                return -1
            }
            onCurrentTextChanged: {
                if (tableModel && itemIndex >= 0 && itemIndex < tableModel.rowCount()) {
                    let modelIndex = tableModel.index(itemIndex, 1)
                    tableModel.setData(modelIndex, currentText, Qt.EditRole)
                    
                    let nameIndex = tableModel.index(itemIndex, 0)
                    let sendIndex = tableModel.index(itemIndex, 2)
                    let rawName = tableModel.data(nameIndex, Qt.DisplayRole) || ""
                    let paramName = rawName.replace(/\s*\(.*\)\s*$/, "") // Xóa phần "(xxbyte)"
                    let send = tableModel.data(sendIndex, Qt.DisplayRole) === "1"
                    
                    mainWindowController.updateCsvValue(deviceType, command, paramName, currentText, send)
                }
            }
            width: parent.width - 4
            height: parent.height - 4
            anchors.centerIn: parent
        }
    }

    // Tiêu đề chính
    Rectangle {
        id: titleBar
        x: 0
        y: 103 // Cách top 103px
        width: parent.width
        height: headerHeight
        color: "#2c3e50"
        
        Text {
            anchors.centerIn: parent
            text: "MODIFY PARAMETER"
            color: "white"
            font.bold: true
            font.pixelSize: 14
        }
    }

    // ScrollView cho cuộn ngang
    ScrollView {
        id: scrollView
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 5
        
        ScrollBar.horizontal.policy: ScrollBar.AsNeeded
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff // Chỉ cuộn ngang
        
        contentWidth: gridContainer.width
        contentHeight: gridContainer.height

        // Container chính cho grid
        Item {
            id: gridContainer
            width: numberOfColumns * (totalItemWidth + spacing) + spacing
            height: scrollView.height

            // Tạo các cột
            Repeater {
                model: numberOfColumns
                
                delegate: Item {
                    id: columnContainer
                    x: index * (totalItemWidth + spacing) + spacing
                    y: 0
                    width: totalItemWidth
                    height: parent.height
                    
                    property int columnIndex: index
                    
                    // Header cho cột này
                    Row {
                        id: columnHeader
                        x: 0
                        y: 10
                        spacing: 0
                        
                        Rectangle {
                            width: nameWidth
                            height: headerHeight
                            color: "#34495e"
                            border.color: "#2c3e50"
                            border.width: 1
                            Text {
                                anchors.centerIn: parent
                                text: "Name Parameter"
                                color: "white"
                                font.bold: true
                                font.pixelSize: 11
                            }
                        }
                        
                        Rectangle {
                            width: valueWidth
                            height: headerHeight
                            color: "#34495e"
                            border.color: "#2c3e50"
                            border.width: 1
                            Text {
                                anchors.centerIn: parent
                                text: "Value"
                                color: "white"
                                font.bold: true
                                font.pixelSize: 11
                            }
                        }
                        
                        Rectangle {
                            width: sendWidth
                            height: headerHeight
                            color: "#34495e"
                            border.color: "#2c3e50"
                            border.width: 1
                            Text {
                                anchors.centerIn: parent
                                text: "Send"
                                color: "white"
                                font.bold: true
                                font.pixelSize: 11
                            }
                        }
                    }
                    
                    // Data rows cho cột này
                    Column {
                        anchors.top: columnHeader.bottom
                        anchors.left: parent.left
                        spacing: 0
                        
                        Repeater {
                            model: maxRowsPerColumn
                            
                            delegate: Item {
                                width: totalItemWidth
                                height: itemHeight
                                
                                property int rowIndex: index
                                property int itemIndex: columnIndex * maxRowsPerColumn + rowIndex
                                property bool hasData: itemIndex < totalItems
                                
                                visible: hasData
                                
                                Row {
                                    spacing: 0
                                    
                                    // Name cell
                                    Rectangle {
                                        width: nameWidth
                                        height: itemHeight
                                        border.color: "#bdc3c7"
                                        border.width: 1
                                        color: "#ecf0f1"
                                        
                                        Text {
                                            anchors.centerIn: parent
                                            text: {
                                                if (hasData && tableModel && itemIndex < tableModel.rowCount()) {
                                                    let nameIndex = tableModel.index(itemIndex, 0)
                                                    return tableModel.data(nameIndex, Qt.DisplayRole) || ""
                                                }
                                                return ""
                                            }
                                            font.pixelSize: 9
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                    }
                                    
                                    // Value cell
                                    Rectangle {
                                        width: valueWidth
                                        height: itemHeight
                                        border.color: "#bdc3c7"
                                        border.width: 1
                                        color: "white"
                                        
                                        property int itemIndex: parent.parent.itemIndex
                                        
                                        Loader {
                                            anchors.fill: parent
                                            anchors.margins: 2
                                            
                                            sourceComponent: {
                                                if (hasData && tableModel && itemIndex < tableModel.rowCount()) {
                                                    let nameIndex = tableModel.index(itemIndex, 0)
                                                    let paramType = tableModel.data(nameIndex, Qt.UserRole + 1) || "text"
                                                    
                                                    if (paramType === "option") return optionComponent
                                                    else if (paramType === "parameter") return stateComponent
                                                    else return textFieldComponent
                                                }
                                                return textFieldComponent
                                            }
                                        }
                                    }
                                    
                                    // Send cell
                                    Rectangle {
                                        width: sendWidth
                                        height: itemHeight
                                        border.color: "#bdc3c7"
                                        border.width: 1
                                        color: "#f8f9fa"
                                        
                                        CheckBox {
                                            anchors.centerIn: parent
                                            checked: {
                                                if (hasData && tableModel && itemIndex < tableModel.rowCount()) {
                                                    let sendIndex = tableModel.index(itemIndex, 2)
                                                    return tableModel.data(sendIndex, Qt.DisplayRole) === "1"
                                                }
                                                return false
                                            }
                                            onCheckedChanged: {
                                                if (hasData && tableModel && itemIndex < tableModel.rowCount()) {
                                                    let modelIndex = tableModel.index(itemIndex, 2)
                                                    tableModel.setData(modelIndex, checked ? "1" : "0", Qt.EditRole)
                                                    
                                                    let nameIndex = tableModel.index(itemIndex, 0)
                                                    let valueIndex = tableModel.index(itemIndex, 1)
                                                    let paramName = tableModel.data(nameIndex, Qt.DisplayRole) || ""
                                                    let paramValue = tableModel.data(valueIndex, Qt.DisplayRole) || ""
                                                    
                                                    mainWindowController.updateCsvValue(deviceType, command, paramName, paramValue, checked)
                                                }
                                            }
                                            scale: 0.8
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Connections
    Connections {
        target: mainWindowController
        function onDeviceChanged(newDevice, newCommand) {
            deviceType = newDevice
            command = newCommand
            tableModel = mainWindowController.loadTableModel(deviceType, command)
        }
    }

    Component.onCompleted: {
        tableModel = mainWindowController.loadTableModel(deviceType, command)
        console.log("Table model loaded with rows:", tableModel ? tableModel.rowCount() : 0)
        console.log("Max rows per column:", maxRowsPerColumn)
        console.log("Number of columns needed:", numberOfColumns)
    }
}