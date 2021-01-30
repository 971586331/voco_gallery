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
    title: "Qt Quick Controls 2"

    property string scan_state : qsTr("点击按钮开始寻找设备")
    property string connect_status : qsTr("未连接到设备")
    property string led_color : "black"
    property string current_user_name : "未选择"
    property string current_wristband_addr : ""

    Settings {
        id: settings
        property string style: "Default"
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
//                        ListElement { title: "BusyIndicator"; source: "qrc:/pages/BusyIndicatorPage.qml" }
//                        ListElement { title: "Button"; source: "qrc:/pages/ButtonPage.qml" }
//                        ListElement { title: "CheckBox"; source: "qrc:/pages/CheckBoxPage.qml" }
//                        ListElement { title: "ComboBox"; source: "qrc:/pages/ComboBoxPage.qml" }
//                        ListElement { title: "DelayButton"; source: "qrc:/pages/DelayButtonPage.qml" }
//                        ListElement { title: "Dial"; source: "qrc:/pages/DialPage.qml" }
//                        ListElement { title: "Dialog"; source: "qrc:/pages/DialogPage.qml" }
//                        ListElement { title: "Delegates"; source: "qrc:/pages/DelegatePage.qml" }
//                        ListElement { title: "Frame"; source: "qrc:/pages/FramePage.qml" }
//                        ListElement { title: "GroupBox"; source: "qrc:/pages/GroupBoxPage.qml" }
//                        ListElement { title: "PageIndicator"; source: "qrc:/pages/PageIndicatorPage.qml" }
//                        ListElement { title: "ProgressBar"; source: "qrc:/pages/ProgressBarPage.qml" }
//                        ListElement { title: "RadioButton"; source: "qrc:/pages/RadioButtonPage.qml" }
//                        ListElement { title: "RangeSlider"; source: "qrc:/pages/RangeSliderPage.qml" }
//                        ListElement { title: "ScrollBar"; source: "qrc:/pages/ScrollBarPage.qml" }
//                        ListElement { title: "ScrollIndicator"; source: "qrc:/pages/ScrollIndicatorPage.qml" }
//                        ListElement { title: "Slider"; source: "qrc:/pages/SliderPage.qml" }
//                        ListElement { title: "SpinBox"; source: "qrc:/pages/SpinBoxPage.qml" }
//                        ListElement { title: "StackView"; source: "qrc:/pages/StackViewPage.qml" }
//                        ListElement { title: "SwipeView"; source: "qrc:/pages/SwipeViewPage.qml" }
//                        ListElement { title: "Switch"; source: "qrc:/pages/SwitchPage.qml" }
//                        ListElement { title: "TabBar"; source: "qrc:/pages/TabBarPage.qml" }
//                        ListElement { title: "TextArea"; source: "qrc:/pages/TextAreaPage.qml" }
//                        ListElement { title: "TextField"; source: "qrc:/pages/TextFieldPage.qml" }
//                        ListElement { title: "ToolTip"; source: "qrc:/pages/ToolTipPage.qml" }
//                        ListElement { title: "Tumbler"; source: "qrc:/pages/TumblerPage.qml" }
                    }

                    ScrollIndicator.vertical: ScrollIndicator { }
                }
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: Pane {
            id: pane


            StatusIndicator {
                id: connect_status_led
                anchors.left : parent.left
                anchors.leftMargin : 10
                anchors.top : parent.top
                anchors.topMargin : 10
                color: window.led_color
                active: true

            }
            Label {
                anchors.left : connect_status_led.right
                anchors.leftMargin : 20
                anchors.verticalCenter: connect_status_led.verticalCenter
                text: connect_status
                font.pixelSize: 16
            }
            Button
            {
                text: "test"
                anchors.top:  connect_status_led.bottom
                anchors.topMargin: 20
                onClicked:
                {
                    mainwindow.button_test()
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
