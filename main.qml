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
import QtCharts 2.14
import "./mypages"
import "./QChart"
import "qrc:/QChart/QChart.js" as Charts
import "qrc:/QChart/QChartGallery.js" as ChartsData

ApplicationWindow {
    id: window
    width: 480
    height: 640
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
    property string device_state : "离线状态"
    property bool wb_is_connect : false
    property string heart_rate

    property bool calibration_busy1
    property bool calibration_busy2
    property bool calibration_busy3
    property bool calibration_busy4
    property string calibration_lable1 : qsTr("未校准")
    property string calibration_lable2 : qsTr("未校准")
    property string calibration_lable3 : qsTr("未校准")
    property string calibration_lable4 : qsTr("未校准")

    property var sensor_data_qml

    property var sensor_name_array : [
    "VO2 in ml/min/kg:",
    "Breath Rate in breaths/minute:",
    "O2%:",
    "Tidal Volume in Litre:",
    "Total Calories Burned:",
    "Flow in L/s:",
    "VE in L/min:",
    "CO2%:",
    "VCO2 in ml/min:",
    "RER:",
    "ETO2 in %:",
    "ETCO2 in %::",
    "Pressure in kPa:",
    "Temperature in °C:",
    "RH in %:"];
    property var sensor_data_array : [
    sensor_data_qml.vo2,
    sensor_data_qml.br,
    sensor_data_qml.o2,
    sensor_data_qml.tvl,
    sensor_data_qml.tcb,
    sensor_data_qml.flow,
    sensor_data_qml.ve,
    sensor_data_qml.co2,
    sensor_data_qml.vco2,
    sensor_data_qml.rer,
    sensor_data_qml.eto2,
    sensor_data_qml.etco2,
    sensor_data_qml.pressure,
    sensor_data_qml.temp,
    sensor_data_qml.rh];

    Connections {
        target: buletooth
        onK50_stateChanged:
        {
//            console.log("k50 state = ", buletooth.k50_state_1, "-", buletooth.k50_state_2)
            switch(buletooth.k50_state_1)
            {
            case 0x00: device_state = "离线状态"; enter_monitor_button.enabled = false; break;
            case 0x01: device_state = "初始状态"; break;
            case 0x02: device_state = "预热中，剩余 " + buletooth.warm_up_remaining_time + " 分钟";
                buletooth.send_weight_timestamp(); break;
            case 0x03: device_state = "预热完成，但没收到体重数据和时间戳"; break;
            case 0x04: device_state = "准备就绪"; enter_monitor_button.enabled = true; break;
            case 0x05: device_state = "正在接收传感器数据包"; break;
            case 0x06: device_state = "暂停接收传感器数据包"; break;
            default:break;
            }

            switch(buletooth.k50_state_2 & 0x03)
            {
            case 0x00:calibration_lable1 = qsTr("未校准");calibration_busy1 = false;break;
            case 0x01:calibration_lable1 = qsTr("正在校准");calibration_busy1 = true;break;
            case 0x02:calibration_lable1 = qsTr("校准完成");calibration_busy1 = false;break;
            default:break;
            }

            switch((buletooth.k50_state_2 >> 2) & 0x03)
            {
            case 0x00:calibration_lable2 = qsTr("未校准");calibration_busy2 = false;break;
            case 0x01:;calibration_lable2 = qsTr("正在校准");calibration_busy2 = true;break;
            case 0x02:calibration_lable2 = qsTr("校准完成");calibration_busy2 = false;break;
            default:break;
            }

            switch((buletooth.k50_state_2 >> 4) & 0x03)
            {
            case 0x00:calibration_lable3 = qsTr("未校准");calibration_busy3 = false;break;
            case 0x01:calibration_lable3 = qsTr("正在校准");calibration_busy3 = true;break;
            case 0x02:calibration_lable3 = qsTr("校准完成");calibration_busy3 = false;break;
            default:break;
            }

            switch((buletooth.k50_state_2 >> 6) & 0x03)
            {
            case 0x00:calibration_lable4 = qsTr("未校准");calibration_busy4 = false;break;
            case 0x01:calibration_lable4 = qsTr("正在校准");calibration_busy4 = true;break;
            case 0x02:calibration_lable4 = qsTr("校准完成");calibration_busy4 = false;break;
            default:break;
            }
        }

        onSensor_dataChanged:
        {
//            console.log("onSensor_dataChanged")
            sensor_data_qml = buletooth.sensor_data
        }
    }

    ConfirmDialog
    {
        id: permissionDialog
        i_title: "错误"
        i_message: "程序已过期，请安装新版本！"
        standardButtons: Dialog.Ok
        onAccepted:
        {
//            console.log("App is not authorized~~~~~~~~~~~~~~~")
            Qt.quit()
        }
    }

    function permission(msg)
    {
        permissionDialog.open()
        return "value"
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

    ConfirmDialog
    {
        id: confirmDialog
        i_title: "确认操作"
        i_message: "是否要退出监控？(这样会停止采集设备的数据)"
        onAccepted:
        {
            // 停止数据采集
            if( buletooth.k50_state_1 === 0x05 )
            {
                buletooth.stop_data_collection()
            }
            stackView.pop()
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mian_pane1

        Pane {
            id: mian_pane2
            anchors.fill: parent

            TabBar {
                id: tabBar
                anchors.top: parent.top
                width: parent.width
                height: parent.height/10
                currentIndex: view.currentIndex

                TabButton {text: qsTr("数值")}
                TabButton {text: qsTr("图表")}
                TabButton {text: qsTr("表盘")}
            }

            SwipeView {
                id: view
                currentIndex: tabBar.currentIndex
                anchors.top: tabBar.bottom
                width: parent.width
                height: parent.height/10*8

                Pane {
                    width: view.width
                    height: view.height

                    GridLayout {
                        id: sensor_data_grid
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        x: 20
                        width: parent.width - 40
                        rowSpacing: 5
                        columns: 2
                        rows: 16
                        Label {text: qsTr("Timestamp:")}
                        Label {text: sensor_data_qml.time}
                        Label {text: qsTr("VO2 in ml/min/kg:")}
                        Label {text: sensor_data_qml.vo2}
                        Label {text: qsTr("Breath Rate in breaths/minute:")}
                        Label {text: sensor_data_qml.br}
                        Label {text: qsTr("O2%:")}
                        Label {text: sensor_data_qml.o2}
                        Label {text: qsTr("Tidal Volume in Litre:")}
                        Label {text: sensor_data_qml.tvl}
                        Label {text: qsTr("Total Calories Burned:")}
                        Label {text: sensor_data_qml.tcb}
                        Label {text: qsTr("Flow in L/s:")}
                        Label {text: sensor_data_qml.flow}
                        Label {text: qsTr("VE in L/min:")}
                        Label {text: sensor_data_qml.ve}
                        Label {text: qsTr("CO2%:")}
                        Label {text: sensor_data_qml.co2}
                        Label {text: qsTr("VCO2 in ml/min:")}
                        Label {text: sensor_data_qml.vco2}
                        Label {text: qsTr("RER:")}
                        Label {text: sensor_data_qml.rer}
                        Label {text: qsTr("ETO2 in %:")}
                        Label {text: sensor_data_qml.eto2}
                        Label {text: qsTr("ETCO2 in %:")}
                        Label {text: sensor_data_qml.etco2}
                        Label {text: qsTr("Pressure in kPa:")}
                        Label {text: sensor_data_qml.pressure}
                        Label {text: qsTr("Temperature in °C:")}
                        Label {text: sensor_data_qml.temp}
                        Label {text: qsTr("RH in %:")}
                        Label {text: sensor_data_qml.rh}
                    }
                }
                Pane {
                    width: view.width
                    height: view.height

                    Label {
                       id: chart_label
                       anchors.left: parent.left
                       anchors.top: parent.top
                       anchors.leftMargin: 10
                       anchors.topMargin: 10
                       text: qsTr("选择传感器：")
                   }

                   ComboBox
                   {
                       anchors.left: chart_label.right
                       anchors.verticalCenter: chart_label.verticalCenter
                       anchors.right: parent.right
                       anchors.rightMargin: 10
                       model: sensor_name_array
                   }

                   ChartView {
                       id:chartView
                       anchors.top: chart_label.bottom
                       anchors.topMargin: 10
                       anchors.bottom: parent.bottom
                       width: parent.width
                       DateTimeAxis
                       {
                           id:axisX
                           format: "HH:mm:ss"; //"yyyy MMM dd";HH:mm:ss
                        //   tickCount: 6
                       }
                       ValueAxis {
                           id:axisY
                           max: 1
                           min: -1
                       }
                       LineSeries {
                           id:spline;
                           axisX: axisX
                           axisY: axisY
                           color: "red"
                           name: "含量"
                           width: 1
                           pointsVisible: true
                       }
                   }

//                   Chart {
//                        id: chart_line;
//                        anchors.top: chart_label.bottom
//                        anchors.topMargin: 10
//                        anchors.bottom: parent.bottom
//                        width: parent.width
//                        chartAnimated: true;
//                        chartAnimationEasing: Easing.InOutElastic;
//                        chartAnimationDuration: 2000;
//                        chartData: ChartsData.ChartLineData;
//                        chartType: Charts.ChartType.LINE;
//                    }
                }
                Pane {
                    width: view.width
                    height: view.height

                    Label {
                       id: panel_label
                       anchors.left: parent.left
                       anchors.top: parent.top
                       anchors.leftMargin: 10
                       anchors.topMargin: 10
                       text: qsTr("选择传感器：")
                   }

                   ComboBox
                   {
                       anchors.left: panel_label.right
                       anchors.verticalCenter: panel_label.verticalCenter
                       anchors.right: parent.right
                       anchors.rightMargin: 10
                       model: sensor_name_array
                   }

                   CircularGauge {
                       id: tachometer
                       width: parent.width * 0.6
                       height: parent.height * 0.6
                       value: valueSource.rpm
                       maximumValue: 100
                       anchors.verticalCenter: parent.verticalCenter
                       anchors.horizontalCenter: parent.horizontalCenter

                       style: TachometerStyle {}
                   }
                }
            }
            Button
            {
                id : quit_monitor_button
                text: "退出监控"
                anchors.bottom:  parent.bottom
                anchors.bottomMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked:
                {
                    confirmDialog.open();
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
            Image {
                id: heart
                width: parent.width * 0.5
                height: parent.height * 0.3
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: "images/heart.png"
                smooth: true
                antialiasing: true
                fillMode: Image.Stretch

                SequentialAnimation{
                    id: heartAnim
                    running: wb_is_connect
                    loops: Animation.Infinite
                    alwaysRunToEnd: true
                    PropertyAnimation { target: heart; property: "scale"; to: 1.2; duration: 500; easing.type: Easing.InQuad }
                    PropertyAnimation { target: heart; property: "scale"; to: 1.0; duration: 500; easing.type: Easing.OutQuad }
                }
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "心率：" + heart_rate
                font.pixelSize: 16
                visible: wb_is_connect
            }
            Label {
                id: label_device_state
                anchors.bottom: label_device_state1.top
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: "设备工作状态："
                font.pixelSize: 16
            }
            Label {
                id: label_device_state1
                anchors.bottom: enter_monitor_button.top
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: device_state
                font.pixelSize: 16
            }
            Button
            {
                id : enter_monitor_button
                enabled: false
                text: "进入监控"
                anchors.bottom:  parent.bottom
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked:
                {
                    if( (buletooth.k50_state_1 !== 0x05) && (buletooth.k50_state_1 !== 0x00) )
                    {
                        buletooth.start_data_collection()
                    }
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

            Label {
                width: aboutDialog.availableWidth
                text: "应用过期时间：" + "2021-03-31 12:00:00"
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }
}
