#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QVariant>
#include <QQmlApplicationEngine>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>



class Bluetooth : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant devices READ get_devices_list NOTIFY devicesChanged)

public:
    Bluetooth(QObject *object);

    Q_INVOKABLE QString get_local_name();
    Q_INVOKABLE QString get_local_address();
    Q_INVOKABLE void ble_start_scan();
    Q_INVOKABLE void ble_stop_scan();
    Q_INVOKABLE void connect_device(const QString &address);
    QVariant get_devices_list();
    bool device_is_exist(QList<QObject*> &devices, const QBluetoothDeviceInfo &info);

    QList<QObject*> m_devices;

    QObject *g_rootObject;
    QBluetoothDeviceInfo currentDeviceInfo;
    void ble_connect_device(QBluetoothDeviceInfo info);

    QLowEnergyController *m_control = nullptr;

    QLowEnergyService *m_hr_service = nullptr;
    QLowEnergyDescriptor m_hr_notificationDesc;

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

private slots:
    void addBlueToothDevicesToList(const QBluetoothDeviceInfo &info);
    void slot_device_finished();
    void slot_serviceDiscovered(const QBluetoothUuid &gatt);
    void slot_discoveryFinished();

    void slot_hr_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_hr_updateHeartRateValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

    void slot_1a00_serviceStateChanged(QLowEnergyService::ServiceState s);

    void slot_1b00_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_1b00_updateValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
    QBluetoothLocalDevice *m_localDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;

};

#endif // BLUETOOTH_H
