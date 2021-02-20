
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4

// 连接设备页面

Page {
    id: connect_page

    property int select_index

    // 设备列表的样式
    Rectangle
    {
        height: devices.height
        width: devices.width
        Component
        {
            id: contactDelegate
            Item
            {
                id: box
                height: 80
                width: parent.width

                MouseArea {
                anchors.fill: parent
                    onClicked: {
                        console.log("onClicked = ", modelData.deviceAddress)
                        buletooth.ble_stop_scan()
                        select_index = index
                        window.scan_state = qsTr("正在连接：") + modelData.deviceAddress + "..."
                        buletooth.connect_device(modelData.deviceAddress)
                    }
                }

                Column
                {
                    Text
                    {
                        id: device
                        font.pixelSize: 16
                        text: qsTr("设备名称：") + modelData.deviceName
                        color: "blue"
                    }

                    Text
                    {
                        id: deviceAddress
                        font.pixelSize: 16
                        text: qsTr("设备地址：") + modelData.deviceAddress
                        color: "blue"
                    }
                }
            }
        }
    }

    Column {
        anchors.fill: parent

        Rectangle
        {
            id: rect1
            width: parent.width
            height: parent.height / 10
            Label
            {
                id: label1
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Qt.AlignHCenter
                text: scan_state
                onTextChanged:
                {
//                    console.log("text = ", text)
                }
            }
        }

        Rectangle
        {
            id: rect2
            width: parent.width
            height: parent.height - (rect1.height + rect1.height)
            
            ListView
            {
                id: devices
//                anchors.fill: parent
                height: parent.height
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                model: buletooth.devices
                highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                clip: true
                focus:true
                currentIndex : select_index
//                onModelChanged: devices.currentIndex = select_index

                delegate: contactDelegate
            }
        }

        Rectangle
        {
            id: rect3
            width: parent.width
            height: parent.height / 10
            Button
            {
                id: button1
                text: "搜索设备"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                onClicked:
                {
                    select_index = 0
                    buletooth.ble_start_scan()
                }
            }
        }
    }
}
