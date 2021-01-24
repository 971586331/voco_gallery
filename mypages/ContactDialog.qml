
import QtQuick 2.12
import QtQuick.Controls 2.12

Dialog {
    id: dialog

    signal finished(string name, string age, string height, string weight)

    function createContact() {
        form.userName.clear();
        form.userAge.clear();
        form.userHeight.clear();
        form.userWeight.clear();

        dialog.title = qsTr("添加用户");
        dialog.open();
    }

    function editContact(contact) {
        form.userName.text = contact.fullName;
        form.userAge.text = contact.address;
        form.userHeight.text = contact.city;
        form.userWeight.text = contact.number;

        dialog.title = qsTr("修改用户");
        dialog.open();
    }

    x: parent.width / 2 - width / 2
    y: parent.height / 10

    focus: true
    modal: true
    title: qsTr("添加用户")
    standardButtons: Dialog.Ok | Dialog.Cancel

    contentItem: ContactForm {
        id: form
    }

    onAccepted: finished(form.userName.text, form.userAge.text, form.userHeight.text, form.userWeight.text)
}
