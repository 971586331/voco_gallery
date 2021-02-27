#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QVariant>
#include <QQmlApplicationEngine>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>
#include "user_info.h"
#include <QTimer>

//传感器数据包
#pragma pack(push)  //保存对齐状态
#pragma pack(2)     //设定为1字节对齐
struct sensor_data_t
{
    unsigned int time;
    unsigned short vo2;
    unsigned short br;
    float o2;
    float tvl;
    float tcb;
    float flow;
    float ve;
    float co2;
    float vco2;
    float rer;
    float eto2;
    float etco2;
    float pressure;
    unsigned short temp;
    unsigned short rh;
};
#pragma pack(pop)   //恢复对齐状态


#define UUID_1900   "BBD71900-3F8A-4CCE-BD68-2C3827C286FF"
#define UUID_1901   "BBD71901-3F8A-4CCE-BD68-2C3827C286FF"
#define UUID_1902   "BBD71902-3F8A-4CCE-BD68-2C3827C286FF"
#define UUID_1903   "BBD71903-3F8A-4CCE-BD68-2C3827C286FF"
#define UUID_1908   "BBD71908-3F8A-4CCE-BD68-2C3827C286FF"

#define UUID_1A00   "2FED1A00-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A01   "2FED1A01-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A02   "2FED1A02-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A03   "2FED1A03-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A04   "2FED1A04-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A05   "2FED1A05-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A06   "2FED1A06-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A07   "2FED1A07-1FA6-4459-B24D-F0C6FC694414"
#define UUID_1A08   "2FED1A08-1FA6-4459-B24D-F0C6FC694414"

#define UUID_1B00   "F8011B00-F66E-494E-A672-A1E660829EAE"
#define UUID_1B01   "F8011B01-F66E-494E-A672-A1E660829EAE"
#define UUID_1B02   "F8011B02-F66E-494E-A672-A1E660829EAE"
#define UUID_1B03   "F8011B03-F66E-494E-A672-A1E660829EAE"

class Bluetooth : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant devices READ get_devices_list NOTIFY devicesChanged)
    Q_PROPERTY(QVariant k50_state_1 READ get_k50_state_1 NOTIFY k50_stateChanged)
    Q_PROPERTY(QVariant k50_state_2 READ get_k50_state_2 NOTIFY k50_stateChanged)
    Q_PROPERTY(QVariant warm_up_remaining_time READ get_warm_up_remaining_time NOTIFY k50_stateChanged)
    Q_PROPERTY(QVariantMap sensor_data READ get_sensor_data NOTIFY sensor_dataChanged)

public:
    Bluetooth(QObject *object, user_info ** info);

    Q_INVOKABLE QString get_local_name();
    Q_INVOKABLE QString get_local_address();
    Q_INVOKABLE void ble_start_scan();
    Q_INVOKABLE void ble_stop_scan();
    Q_INVOKABLE int connect_device(const QString &address);
    QVariant get_devices_list();
    bool device_is_exist(QList<QObject*> &devices, const QBluetoothDeviceInfo &info);
    QVariant get_k50_state_1();
    QVariant get_k50_state_2();
    QVariant get_warm_up_remaining_time();
     QVariantMap get_sensor_data();
    Q_INVOKABLE void calibration_1_callback();
    Q_INVOKABLE void calibration_2_callback();
    Q_INVOKABLE void calibration_3_callback();
    Q_INVOKABLE void calibration_4_callback();
    Q_INVOKABLE void start_data_collection();
    Q_INVOKABLE void stop_data_collection();

    QString wb_name;
    QList<QObject*> m_devices;
    quint8 m_k50_state_1;
    quint8 m_k50_state_2;
    quint16 m_warm_up_remaining_time = 0;
    QVariantMap sensor_data_map;

    user_info **active_user = nullptr;
    QObject *g_rootObject;
    QBluetoothDeviceInfo currentDeviceInfo;
    void ble_connect_device(QBluetoothDeviceInfo info);

    QLowEnergyController *m_control = nullptr;

    QLowEnergyService *m_hr_service = nullptr;
    QLowEnergyDescriptor m_hr_notificationDesc;

    QLowEnergyService *m_service_1800 = nullptr;
    QLowEnergyCharacteristic Char_2a00;

    QLowEnergyService *m_service_180a = nullptr;
    QLowEnergyCharacteristic Char_2a29;

    QLowEnergyService *m_service_1900 = nullptr;
    QLowEnergyCharacteristic Char_1901;
    QLowEnergyCharacteristic Char_1902;
    QLowEnergyCharacteristic Char_1903;
    QLowEnergyCharacteristic Char_1904;
    QLowEnergyCharacteristic Char_1905;
    QLowEnergyCharacteristic Char_1906;
    QLowEnergyCharacteristic Char_1907;
    QLowEnergyCharacteristic Char_1908;

    QLowEnergyService *m_service_1a00 = nullptr;
    QLowEnergyCharacteristic Char_1a01;
    QLowEnergyCharacteristic Char_1a02;
    QLowEnergyCharacteristic Char_1a03;
    QLowEnergyCharacteristic Char_1a04;
    QLowEnergyCharacteristic Char_1a05;
    QLowEnergyCharacteristic Char_1a06;
    QLowEnergyCharacteristic Char_1a07;
    QLowEnergyCharacteristic Char_1a08;

    QLowEnergyService *m_service_1b00 = nullptr;
    QLowEnergyDescriptor m_1b01_notificationDesc;
    QLowEnergyCharacteristic Char_1b01;
    QLowEnergyDescriptor m_1b02_notificationDesc;
    QLowEnergyCharacteristic Char_1b02;
    QLowEnergyDescriptor m_1b03_notificationDesc;
    QLowEnergyCharacteristic Char_1b03;

signals:
    void devicesChanged();
    void k50_stateChanged();
    void sensor_dataChanged();

private slots:
    void addBlueToothDevicesToList(const QBluetoothDeviceInfo &info);
    void slot_device_finished();
    void slot_serviceDiscovered(const QBluetoothUuid &gatt);
    void slot_discoveryFinished();

    void slot_1800_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_1800_characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

    void slot_1900_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_1900_characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

    void slot_hr_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_hr_updateHeartRateValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

    void slot_1a00_serviceStateChanged(QLowEnergyService::ServiceState s);

    void slot_1b00_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_1b00_updateValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

    void on_wb_state_time_timeout();

private:
    QBluetoothLocalDevice *m_localDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;

    QTimer *wb_state_time;

};

#endif // BLUETOOTH_H
