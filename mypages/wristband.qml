
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4

// 设置用户手环页面

Page {
    id: wristband_page

    property int select_index
    property string select_wristband_addr : ""

    Dialog {
        id: dialog

        x: parent.width / 2 - width / 2
        y: parent.height / 10

        focus: true
        modal: true
        title: qsTr("添加用户")
        Text {
            text: qsTr("是否要将连接的手环设置为：\n") + select_wristband_addr
        }
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted:
        {
            window.current_wristband_addr = select_wristband_addr
            mainwindow.set_wristband_addr(select_wristband_addr);
        }
    }

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
                        buletooth.ble_stop_scan()
                        select_index = index
                        select_wristband_addr = modelData.deviceAddress
                        dialog.open();
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
                text: "当前设置：" + window.current_wristband_addr
                onTextChanged:
                {
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
                currentIndex : connect_page.select_index

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
                    connect_page.select_index = 0
                    buletooth.ble_start_scan()
                }
            }
        }
    }
}
