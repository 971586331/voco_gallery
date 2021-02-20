
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4

// 校准页面

Page {
    id: calibration_page

    GridLayout
    {
        columns: 4

        Label {
            text: qsTr("O2 Span cal：")
        }

        Label {
            id : lable1
            text: calibration_lable1
        }

        BusyIndicator
        {
            id : busy1
            running: calibration_busy1
        }

        Button {
            text: qsTr("校准")
            onClicked: buletooth.calibration_1_callback()
        }

        Label {
            text: qsTr("Flow Zero cal：")
        }

        Label {
            id : lable2
            text: calibration_lable2
        }

        BusyIndicator
        {
            id : busy2
            running: calibration_busy2
        }

        Button {
            text: qsTr("校准")
            onClicked: buletooth.calibration_2_callback()
        }

        Label {
            text: qsTr("OC2 Zero cal：")
        }

        Label {
            id : lable3
            text: calibration_lable3
        }

        BusyIndicator
        {
            id : busy3
            running: calibration_busy3
        }

        Button {
            text: qsTr("校准")
            onClicked: buletooth.calibration_3_callback()
        }

        Label {
            text: qsTr("CO2 Span cal：")
        }

        Label {
            id : lable4
            text: calibration_lable4
        }

        BusyIndicator
        {
            id : busy4
            running: calibration_busy4
        }

        Button {
            text: qsTr("校准")
            onClicked: buletooth.calibration_4_callback()
        }
    }
}
