
import QtQuick 2.12
import QtQuick.Controls 2.12

Dialog {
    property string i_title : "标题"
    property string i_message : "消息"

    x: parent.width / 2 - width / 2
    y: parent.height / 5

    focus: true
    modal: true
    title: i_title
    standardButtons: Dialog.Ok | Dialog.Cancel

    Label {
        text: i_message
    }
}
