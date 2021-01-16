#include "bluetooth.h"
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <deviceinfo.h>

Bluetooth::Bluetooth(QObject *obj) : g_rootObject(obj)
{

    m_localDevice = new QBluetoothLocalDevice();
    discoveryAgent =new QBluetoothDeviceDiscoveryAgent();
    connect(discoveryAgent,
                SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
                this,
                SLOT(addBlueToothDevicesToList(QBluetoothDeviceInfo))
                );
    connect(discoveryAgent,SIGNAL(finished()),this,SLOT(slot_device_finished()));
}

/**
 * @brief Bluetooth::name   获得主机名
 * @return
 */
QString Bluetooth::get_local_name()
{
    qDebug() << "run name()" << m_localDevice->name();
    return m_localDevice->name();
}

/**
 * @brief Bluetooth::address    获得主机地址
 * @return
 */
QString Bluetooth::get_local_address()
{
    qDebug() << "run address()" << m_localDevice->address().toString();
    return m_localDevice->address().toString();
}

/**
 * @brief Bluetooth::ble_start_scan 开始扫描
 */
void Bluetooth::ble_start_scan()
{
    qDebug() << "run ble_start_scan()";
    discoveryAgent->start();
    g_rootObject->setProperty("scan_state", "正在扫描...");
}

/**
 * @brief Bluetooth::ble_stop_scan  停止扫描
 */
void Bluetooth::ble_stop_scan()
{
    qDebug() << "run ble_stop_scan()";
    discoveryAgent->stop();
}

bool Bluetooth::device_is_exist(QList<QObject*> &devices, const QBluetoothDeviceInfo &info)
{
    for ( int i=0; i!=devices.size(); ++i )
    {
        DeviceInfo *info_1 = (DeviceInfo *)devices.at(i);
        if( info.address().toString() == info_1->getAddress() )
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Bluetooth::addBlueToothDevicesToList  发现设备
 * @param info
 */
void Bluetooth::addBlueToothDevicesToList(const QBluetoothDeviceInfo &info)
{
    qDebug() << "address:" << info.address().toString() << "name" << info.name();
    // If device is LowEnergy-device, add it to the list
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        bool ret = device_is_exist(m_devices, info);
        if( ret == false )
        {
            m_devices.append(new DeviceInfo(info));
            emit devicesChanged();
        }
    }
}

void Bluetooth::slot_device_finished()
{
    qDebug() << "scan ok!";
    ble_stop_scan();
    g_rootObject->setProperty("scan_state", "扫描完成");
}

/**
 * @brief DeviceFinder::devices 获得设备列表
 * @return
 */
QVariant Bluetooth::get_devices_list()
{
    return QVariant::fromValue(m_devices);
}

