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
import QtQuick.Extras 1.4

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
    //                            anchors.top: parent.top
    //                            anchors.topMargin: parent.height * 0.05
    //                            anchors.leftMargin: parent.width * 0.05
    //                            anchors.left: parent.left
                        color: "blue"
                    }

                    Text
                    {
                        id: deviceAddress
                        font.pixelSize: 16
                        text: qsTr("设备地址：") + modelData.deviceAddress
    //                            anchors.bottom: parent.bottom
    //                            anchors.bottomMargin: parent.height * 0.05
    //                            anchors.rightMargin: parent.width * 0.05
    //                            anchors.right: parent.right
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
                currentIndex : connect_page.select_index
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

            Button
            {
                id: button2
                text: "test"
                anchors.left : button1.right
                anchors.verticalCenter: parent.verticalCenter
                onClicked:
                {
                    console.log("select_index = ", connect_page.select_index)
                }
            }
        }
    }
}
