
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4

// 用户信息页面

Page {
    id: user_info_page

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
                height: weight.y + weight.height - name.y + 10
                width: parent.width

                MouseArea {
                anchors.fill: parent
                    onClicked: {
                        console.log("onClicked = ")
                        select_index = index
                    }
                }

                Column
                {
                    Text
                    {
                        id: name
                        font.pixelSize: 16
                        text: qsTr("姓名：") + modelData.name
                        color: "blue"
                    }
                    Text
                    {
                        id: age
                        font.pixelSize: 16
                        text: qsTr("年龄：") + modelData.age
                        anchors.left: name.left
                        anchors.leftMargin: 20
                        color: "blue"
                    }
                    Text
                    {
                        id: height
                        font.pixelSize: 16
                        text: qsTr("身高(cm)：") + modelData.height
                        anchors.left: name.left
                        anchors.leftMargin: 20
                        color: "blue"
                    }
                    Text
                    {
                        id: weight
                        font.pixelSize: 16
                        text: qsTr("体重(kg)：") + modelData.weight
                        anchors.left: name.left
                        anchors.leftMargin: 20
                        color: "blue"
                    }
                }
            }
        }
    }

    ContactDialog {
        id: contactDialog
        onFinished:
        {
            var flag = mainwindow.add_user(name, age, height, weight)
            if( flag === false )
                msgdialog_warning.openMsg()
        }
    }

    MsgDialog {
        id: msgdialog_warning
        tipText : qsTr("添加失败，用户名称重复或用户名称为空！")
        tipImage : "qrc:/images/warning.png"
    }

    MsgDialog {
        id: msgdialog_ok
        tipText : qsTr("用户切换成功！")
        tipImage : "qrc:/images/ok.png"
    }

    MsgDialog {
        id: msgdialog_ok_2
        tipText : qsTr("用户已删除！")
        tipImage : "qrc:/images/ok.png"
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
                text: "用户列表"
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
                model: mainwindow.user_info
                highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                clip: true
                focus:true
                currentIndex : user_info_page.select_index
//                onModelChanged: devices.currentIndex = connect_page.select_index

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
                id: button_add_user
                text: "添加用户"
                anchors.right: button_del_user.left
                anchors.rightMargin: 20
                anchors.verticalCenter: button_del_user.verticalCenter
                onClicked:
                {
                    contactDialog.createContact()
                }
            }

            Button
            {
                id: button_del_user
                text: "删除用户"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked:
                {
                    var flag = mainwindow.delete_user(select_index)
                    if( flag === true )
                        msgdialog_ok_2.openMsg()
                }
            }

            Button
            {
                id: button_switch_user
                text: "切换用户"
                anchors.left : button_del_user.right
                anchors.leftMargin: 20
                anchors.verticalCenter: button_del_user.verticalCenter
                onClicked:
                {
                    var flag = mainwindow.switch_user(select_index)
                    if( flag === true )
                        msgdialog_ok.openMsg()
                }
            }
        }
    }
}
