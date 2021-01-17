#include "bluetooth.h"
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <deviceinfo.h>
#include <QLowEnergyController>
#include <QtEndian>
#include <QRandomGenerator>

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
    m_devices.clear();
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
    g_rootObject->setProperty("scan_state", "扫描完成");
}

/**
 * @brief Bluetooth::device_is_exist    查找设备是否存在
 * @param devices   已知设备列表
 * @param info      新设备信息
 * @return
 */
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

/**
 * @brief Bluetooth::slot_device_finished   扫描设备完成
 */
void Bluetooth::slot_device_finished()
{
    qDebug() << "scan ok!";
    ble_stop_scan();
}

/**
 * @brief DeviceFinder::devices 获得设备列表
 * @return
 */
QVariant Bluetooth::get_devices_list()
{
    return QVariant::fromValue(m_devices);
}

/**
 * @brief Bluetooth::connect_device 连接到设备
 * @param address   设备Mac地址
 */
void Bluetooth::connect_device(const QString &address)
{
    ble_stop_scan();

    for ( int i=0; i!=m_devices.size(); ++i )
    {
        DeviceInfo *info = (DeviceInfo *)m_devices.at(i);
        if( info->getAddress() == address )
        {
            currentDeviceInfo = info->getDevice();
            // 找到这个设备的信息
            // Make connections
            //! [Connect-Signals-1]
            m_control = QLowEnergyController::createCentral(currentDeviceInfo, this);
            //! [Connect-Signals-1]
            m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);
            //! [Connect-Signals-2]
            connect(m_control, &QLowEnergyController::serviceDiscovered,
                    this, &Bluetooth::slot_serviceDiscovered);
            connect(m_control, &QLowEnergyController::discoveryFinished,
                    this, &Bluetooth::slot_discoveryFinished);

            connect(m_control, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error),
                    this, [this](QLowEnergyController::Error error)
            {
                Q_UNUSED(error);
                qDebug("无法连接到远端设备。");
                g_rootObject->setProperty("scan_state", tr("连接失败，请点击按钮开始寻找设备并连接！"));
            });

            connect(m_control, &QLowEnergyController::connected, this, [this]()
            {
                qDebug("控制器连接.搜索服务……");
                m_control->discoverServices();
                g_rootObject->setProperty("scan_state", QString("已经连接到:").append(currentDeviceInfo.address().toString()));
            });

            connect(m_control, &QLowEnergyController::disconnected, this, [this]()
            {
                qDebug("LowEnergy控制器断开连接");
                g_rootObject->setProperty("scan_state", tr("连接继开，请点击按钮开始寻找设备并连接！"));
            });

            // Connect
            m_control->connectToDevice();

            break;
        }
    }

    // 没有这个设备
}

/**
 * @brief Bluetooth::slot_serviceDiscovered 发现了新服务
 * @param gatt
 */
void Bluetooth::slot_serviceDiscovered(const QBluetoothUuid &gatt)
{
    // 处理发现到的服务
    if (gatt == QBluetoothUuid(QBluetoothUuid::HeartRate))
    {
        qDebug("Heart Rate service discovered. Waiting for service scan to be done...");
        m_hr_service = m_control->createServiceObject(QBluetoothUuid(QBluetoothUuid::HeartRate), this);

        if (m_hr_service) {
            connect(m_hr_service, &QLowEnergyService::stateChanged, this, &Bluetooth::slot_hr_serviceStateChanged);
            connect(m_hr_service, &QLowEnergyService::characteristicChanged, this, &Bluetooth::slot_hr_updateHeartRateValue);
//            connect(m_hr_service, &QLowEnergyService::descriptorWritten, this, &DeviceHandler::confirmedDescriptorWrite);
            m_hr_service->discoverDetails();
        } else {
            qDebug("心率服务没有找到！");
        }
    }
}

/**
 * @brief Bluetooth::slot_discoveryFinished 发现服务结束
 */
void Bluetooth::slot_discoveryFinished()
{
    // 处理发现到的服务
    qDebug("心率服务查找完成！");
}

/**
 * @brief Bluetooth::serviceStateChanged    心率服务状态改变
 * @param s
 */
void Bluetooth::slot_hr_serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
        case QLowEnergyService::DiscoveringServices:
            qDebug("Discovering services...");
            break;
        case QLowEnergyService::ServiceDiscovered:
        {
            qDebug("Service discovered.");

            const QLowEnergyCharacteristic hrChar = m_hr_service->characteristic(QBluetoothUuid(QBluetoothUuid::HeartRateMeasurement));
            if (!hrChar.isValid()) {
                qDebug("HR Data not found.");
                break;
            }

            m_hr_notificationDesc = hrChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (m_hr_notificationDesc.isValid())
                m_hr_service->writeDescriptor(m_hr_notificationDesc, QByteArray::fromHex("0100"));

            break;
        }
        default : break;
    }
//    emit aliveChanged();
}

/**
 * @brief Bluetooth::slot_hrupdateHeartRateValue 更新心率服务的值
 * @param c
 * @param value
 */
void Bluetooth::slot_hr_updateHeartRateValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != QBluetoothUuid(QBluetoothUuid::HeartRateMeasurement))
        return;

    auto data = reinterpret_cast<const quint8 *>(value.constData());
    quint8 flags = *data;

    //Heart Rate
    int hrvalue = 0;
    if (flags & 0x1) // HR 16 bit? otherwise 8 bit
        hrvalue = static_cast<int>(qFromLittleEndian<quint16>(data[1]));
    else
        hrvalue = static_cast<int>(data[1]);

    qDebug("心率 = %d", hrvalue);
//    addMeasurement(hrvalue);
}

