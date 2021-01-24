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
    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_hr_service = nullptr;
    QLowEnergyDescriptor m_hr_notificationDesc;

    QBluetoothDeviceInfo currentDeviceInfo;
    void ble_connect_device(QBluetoothDeviceInfo info);

signals:
    void devicesChanged();

private slots:
    void addBlueToothDevicesToList(const QBluetoothDeviceInfo &info);
    void slot_device_finished();
    void slot_serviceDiscovered(const QBluetoothUuid &gatt);
    void slot_discoveryFinished();
    void slot_hr_serviceStateChanged(QLowEnergyService::ServiceState s);
    void slot_hr_updateHeartRateValue(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
    QBluetoothLocalDevice *m_localDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;

};

#endif // BLUETOOTH_H
