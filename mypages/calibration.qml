
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4

// 校准页面

Page {
    id: calibration_page

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
