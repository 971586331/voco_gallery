/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import Qt.labs.settings 1.0
import QtQuick.Extras 1.4

ApplicationWindow {
    id: window
    width: 360
    height: 520
    visible: true
    title: "VOCO"

    property string scan_state : qsTr("点击按钮开始寻找设备")
    property string connect_status : qsTr("未连接VOCO设备")
    property string wb_connect_status : qsTr("未连接手环")
    property string wb_name
    property string voco_led_color : "black"
    property string wb_led_color : "black"
    property string current_user_name : qsTr("未选择")
    property string current_wristband_addr : ""

    property bool calibration_busy1
    property bool calibration_busy2
    property bool calibration_busy3
    property bool calibration_busy4
    property string calibration_lable1 : qsTr("未校准")
    property string calibration_lable2 : qsTr("未校准")
    property string calibration_lable3 : qsTr("未校准")
    property string calibration_lable4 : qsTr("未校准")

    Connections {
        target: buletooth
        onK50_stateChanged:
        {
//            console.log("k50 state = ", buletooth.k50_state_1, "-", buletooth.k50_state_2)
            switch(buletooth.k50_state_2 & 0x03)
            {
            case 0x00:
                calibration_lable1 = qsTr("未校准")
                calibration_busy1 = false
                break;
            case 0x01:
                calibration_lable1 = qsTr("正在校准")
                calibration_busy1 = true
                break;
            case 0x02:
                calibration_lable1 = qsTr("校准完成")
                calibration_busy1 = false
                break;
            default:break;
            }

            switch((buletooth.k50_state_2 >> 2) & 0x03)
            {
            case 0x00:
                calibration_lable2 = qsTr("未校准")
                calibration_busy2 = false
                break;
            case 0x01:
                calibration_lable2 = qsTr("正在校准")
                calibration_busy2 = true
                break;
            case 0x02:
                calibration_lable2 = qsTr("校准完成")
                calibration_busy2 = false
                break;
            default:break;
            }

            switch((buletooth.k50_state_2 >> 4) & 0x03)
            {
            case 0x00:
                calibration_lable3 = qsTr("未校准")
                calibration_busy3 = false
                break;
            case 0x01:
                calibration_lable3 = qsTr("正在校准")
                calibration_busy3 = true
                break;
            case 0x02:
                calibration_lable3 = qsTr("校准完成")
                calibration_busy3 = false
                break;
            default:break;
            }

            switch((buletooth.k50_state_2 >> 6) & 0x03)
            {
            case 0x00:
                calibration_lable4 = qsTr("未校准")
                calibration_busy4 = false
                break;
            case 0x01:
                calibration_lable4 = qsTr("正在校准")
                calibration_busy4 = true
                break;
            case 0x02:
                calibration_lable4 = qsTr("校准完成")
                calibration_busy4 = false
                break;
            default:break;
            }
        }
    }

    Settings {
        id: settings
//        property string style: "Default"
        property string style: "Material"
    }

    Shortcut {
        sequences: ["Esc", "Back"]
        enabled: stackView.depth > 1
        onActivated: navigateBackAction.trigger()
    }

    Action {
        id: navigateBackAction
        icon.name: stackView.depth > 1 ? "back" : "drawer"
        onTriggered: {
            if (stackView.depth > 1) {
                stackView.pop()
                listView.currentIndex = -1
            } else {
                drawer.open()
            }
        }
    }

    Shortcut {
        sequence: "Menu"
        onActivated: optionsMenuAction.trigger()
    }

    Action {
        id: optionsMenuAction
        icon.name: "menu"
        onTriggered: optionsMenu.open()
    }

    header: ToolBar {
        Material.foreground: "white"

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                action: navigateBackAction
            }

            Label {
                id: titleLabel
                text: listView.currentItem ? listView.currentItem.text : "VOCO"
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                action: optionsMenuAction

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

                    Action {
                        text: "设置"
                        onTriggered: settingsDialog.open()
                    }
                    Action {
                        text: "关于"
                        onTriggered: aboutDialog.open()
                    }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        interactive: stackView.depth === 1

        Column
        {
            spacing: 10
            Rectangle
            {
                id: drawer_rect1
                width: drawer.width
                height: window.height / 5
                Image {
                    id: drawer_logo
                    width: drawer_rect1.height / 2
                    height: drawer_rect1.height / 2
                    anchors.left: drawer_rect1.left
                    anchors.leftMargin: 30
                    anchors.top: drawer_rect1.top
                    anchors.topMargin: 30
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/images/user.png"
                }
            }

            Rectangle
            {
                id: drawer_rect2
                width: drawer.width
                height: window.height / 10
                Label
                {
                    id:label_name_1
                    anchors.left: drawer_rect2.left
                    anchors.leftMargin: 10
                    anchors.top: drawer_rect2.top
                    text: qsTr("用户名：")
                }
                Label
                {
                    anchors.left: label_name_1.right
                    anchors.leftMargin: 10
                    anchors.verticalCenter: label_name_1.verticalCenter
                    text: current_user_name
                }
            }

            Rectangle
            {
                width: drawer.width
                height: window.height - (drawer_rect2.y + drawer_rect2.height)
                ListView {
                    id: listView

                    focus: true
                    currentIndex: -1
                    anchors.fill: parent

                    delegate: ItemDelegate {
                        width: parent.width
                        text: model.title
                        highlighted: ListView.isCurrentItem
                        onClicked: {
                            listView.currentIndex = index
                            stackView.push(model.source)
                            drawer.close()
                        }
                    }

                    model: ListModel {
                        ListElement {title: "用户管理"; source: "qrc:/mypages/user_info.qml" }
                        ListElement {title: "连接设备"; source: "qrc:/mypages/connect.qml" }
                        ListElement {title: "设置心率手环"; source: "qrc:/mypages/wristband.qml" }
                        ListElement {title: "校准"; source: "qrc:/mypages/calibration.qml" }
                    }

                    ScrollIndicator.vertical: ScrollIndicator { }
                }
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mian_pane1

        Pane {
            id: mian_pane2
            anchors.fill: parent

            Label {
                anchors.left : parent.left
                anchors.leftMargin : 20
                text: "sss"
                font.pixelSize: 16
            }
            Button
            {
                id : quit_monitor_button
                text: "退出监控"
                anchors.bottom:  parent.bottom
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked:
                {

                    stackView.pop()
                }
            }
        }

        Pane {
            id: mian_pane1
            anchors.fill: parent

            StatusIndicator {
                id: connect_status_led
                anchors.left : parent.left
                anchors.leftMargin : 10
                anchors.top : parent.top
                anchors.topMargin : 10
                color: window.voco_led_color
                active: true
            }
            Label {
                anchors.left : connect_status_led.right
                anchors.leftMargin : 20
                anchors.verticalCenter: connect_status_led.verticalCenter
                text: connect_status
                font.pixelSize: 16
            }
            StatusIndicator {
                id: wb_connect_status_led
                anchors.left : parent.left
                anchors.leftMargin : 10
                anchors.top : connect_status_led.bottom
                anchors.topMargin : 10
                color: window.wb_led_color
                active: true
            }
            Label {
                id: wb_connect_status_label
                anchors.left : wb_connect_status_led.right
                anchors.leftMargin : 20
                anchors.verticalCenter: wb_connect_status_led.verticalCenter
                text: window.wb_connect_status + wb_name
                font.pixelSize: 16
            }
            Label {
                anchors.left : parent.left
                anchors.leftMargin : 20
                anchors.bottom: enter_monitor_button.top
                anchors.bottomMargin: 20
                text: "设备工作状态："
                font.pixelSize: 16
            }
            Button
            {
                text: "test"
                anchors.bottom:  parent.bottom
                anchors.bottomMargin: 20
                anchors.left: parent.left
                onClicked:
                {
//                    mainwindow.button_test()
                }
            }
            Button
            {
                id : enter_monitor_button
//                enabled: false
                text: "进入监控"
                anchors.bottom:  parent.bottom
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked:
                {
                    stackView.push(mian_pane2)
                }
            }
        }
    }

    Dialog {
        id: settingsDialog
        x: Math.round((window.width - width) / 2)
        y: Math.round(window.height / 6)
        width: Math.round(Math.min(window.width, window.height) / 3 * 2)
        modal: true
        focus: true
        title: "Settings"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            settings.style = styleBox.displayText
            settingsDialog.close()
        }
        onRejected: {
            styleBox.currentIndex = styleBox.styleIndex
            settingsDialog.close()
        }

        contentItem: ColumnLayout {
            id: settingsColumn
            spacing: 20

            RowLayout {
                spacing: 10

                Label {
                    text: "Style:"
                }

                ComboBox {
                    id: styleBox
                    property int styleIndex: -1
                    model: availableStyles
                    Component.onCompleted: {
                        styleIndex = find(settings.style, Qt.MatchFixedString)
                        if (styleIndex !== -1)
                            currentIndex = styleIndex
                    }
                    Layout.fillWidth: true
                }
            }

            Label {
                text: "Restart required"
                color: "#e41e25"
                opacity: styleBox.currentIndex !== styleBox.styleIndex ? 1.0 : 0.0
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: qsTr("关于")
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: aboutColumn.height

        Column {
            id: aboutColumn
            spacing: 20

            Label {
                width: aboutDialog.availableWidth
                text: qsTr("这个应用是用作连接VOCO设备，控制设备运行，监控设备运行状态。")
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }

            Label {
                width: aboutDialog.availableWidth
                text: "如在使用过程中有疑问，请发邮件至ｘｘｘ"
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }
}
