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
#include <QString>
#include <QBluetoothSocket>

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
            ble_connect_device(currentDeviceInfo);
            break;
        }
    }

    // 没有这个设备
}

void Bluetooth::ble_connect_device(QBluetoothDeviceInfo info)
{
    // Make connections
    //! [Connect-Signals-1]
    m_control = QLowEnergyController::createCentral(info, this);
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
        g_rootObject->setProperty("connect_status", true);
        g_rootObject->setProperty("led_color", "green");
        m_control->discoverServices();
        g_rootObject->setProperty("scan_state", QString("已经连接到:").append(currentDeviceInfo.address().toString()));
    });

    connect(m_control, &QLowEnergyController::disconnected, this, [this]()
    {
        qDebug("LowEnergy控制器断开连接");
        g_rootObject->setProperty("connect_status", false);
        g_rootObject->setProperty("led_color", "black");
        g_rootObject->setProperty("scan_state", tr("连接断开，请点击按钮开始寻找设备并连接！"));
    });

    // Connect
    m_control->connectToDevice();
}

/**
 * @brief Bluetooth::slot_serviceDiscovered 发现了新服务
 * @param gatt
 */
void Bluetooth::slot_serviceDiscovered(const QBluetoothUuid &gatt)
{
    if( gatt.minimumSize() == 2 )
    {
        if (gatt == QBluetoothUuid(QBluetoothUuid::GenericAccess))
        {
            qDebug("服务：通用访问配置");
        }
        if (gatt == QBluetoothUuid(QBluetoothUuid::DeviceInformation))
        {
            qDebug("服务：设备信息");
        }
        if (gatt == QBluetoothUuid(QBluetoothUuid::HeartRate))
        {
            qDebug("服务：心率");
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
    else if( gatt.minimumSize() == 16 )
    {
        if( gatt == QBluetoothUuid(QString("BBD71900-3F8A-4CCE-BD68-2C3827C286FF")) )
        {
            qDebug("服务：手环设备信息");
        }
        if( gatt == QBluetoothUuid(QString("2FED1A00-1FA6-4459-B24D-F0C6FC694414")) )
        {
            qDebug("服务：下发K50数据");
            m_service_1a00 = m_control->createServiceObject(QBluetoothUuid(QString("2FED1A00-1FA6-4459-B24D-F0C6FC694414")), this);
            if (m_service_1a00) {
                connect(m_service_1a00, &QLowEnergyService::stateChanged, this, &Bluetooth::slot_1a00_serviceStateChanged);
                m_service_1a00->discoverDetails();
            } else {
                qDebug("1A00服务没有找到！");
            }
        }
        if( gatt == QBluetoothUuid(QString("F8011B00-F66E-494E-A672-A1E660829EAE")) )
        {
            qDebug("服务：获得K50数据");
            m_service_1b00 = m_control->createServiceObject(QBluetoothUuid(QString("F8011B00-F66E-494E-A672-A1E660829EAE")), this);
            if (m_service_1b00) {
                connect(m_service_1b00, &QLowEnergyService::stateChanged, this, &Bluetooth::slot_1b00_serviceStateChanged);
                connect(m_service_1b00, &QLowEnergyService::characteristicChanged, this, &Bluetooth::slot_1b00_updateValue);
                m_service_1b00->discoverDetails();
            } else {
                qDebug("心率服务没有找到！");
            }
        }
    }
}

/**
 * @brief Bluetooth::slot_discoveryFinished 发现服务结束
 */
void Bluetooth::slot_discoveryFinished()
{
    // 处理发现到的服务
    qDebug("服务查找完成！");
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
            qDebug("hr Discovering services...");
            break;
        case QLowEnergyService::ServiceDiscovered:
        {
            qDebug("hr Service discovered.");

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
}

/**
 * @brief Bluetooth::slot_1a00_serviceStateChanged
 * @param s
 */
void Bluetooth::slot_1a00_serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
        case QLowEnergyService::DiscoveringServices:
            qDebug("1a00 Discovering services...");
            break;
        case QLowEnergyService::ServiceDiscovered:
        {
            qDebug("1a00 Service discovered.");

            Char_1a01 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A01-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a01.isValid())
            {
                qDebug("(error)0x1a01 not found.");
            }
            Char_1a02 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A02-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a02.isValid())
            {
                qDebug("(error)0x1a02 not found.");
            }
            Char_1a03 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A03-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a03.isValid())
            {
                qDebug("(error)0x1a03 not found.");
            }
            Char_1a04 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A04-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a04.isValid())
            {
                qDebug("(error)0x1a04 not found.");
            }
            Char_1a05 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A05-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a05.isValid())
            {
                qDebug("(error)0x1a05 not found.");
            }
            Char_1a06 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A06-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a06.isValid())
            {
                qDebug("(error)0x1a06 not found.");
            }
            Char_1a07 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A07-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a07.isValid())
            {
                qDebug("(error)0x1a07 not found.");
            }
            Char_1a08 = m_service_1a00->characteristic(QBluetoothUuid(QString("2FED1A08-1FA6-4459-B24D-F0C6FC694414")));
            if (!Char_1a08.isValid())
            {
                qDebug("(error)0x1a08 not found.");
            }

            break;
        }
        default : break;
    }
}

/**
 * @brief Bluetooth::slot_1b00_serviceStateChanged
 * @param s
 */
void Bluetooth::slot_1b00_serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
        case QLowEnergyService::DiscoveringServices:
            qDebug("1b00 Discovering services...");
            break;
        case QLowEnergyService::ServiceDiscovered:
        {
            qDebug("1b00 Service discovered.");

            QLowEnergyCharacteristic Char_1b01 = m_service_1b00->characteristic(QBluetoothUuid(QString("F8011B01-F66E-494E-A672-A1E660829EAE")));
            if (!Char_1b01.isValid()) {
                qDebug("1b01 not found.");
                break;
            }
            m_1b01_notificationDesc = Char_1b01.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (m_1b01_notificationDesc.isValid())
                m_service_1b00->writeDescriptor(m_1b01_notificationDesc, QByteArray::fromHex("0100"));

            QLowEnergyCharacteristic Char_1b02 = m_service_1b00->characteristic(QBluetoothUuid(QString("F8011B02-F66E-494E-A672-A1E660829EAE")));
            if (!Char_1b02.isValid()) {
                qDebug("1b02 Data not found.");
                break;
            }
            m_1b02_notificationDesc = Char_1b02.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (m_1b02_notificationDesc.isValid())
                m_service_1b00->writeDescriptor(m_1b02_notificationDesc, QByteArray::fromHex("0100"));

            QLowEnergyCharacteristic Char_1b03 = m_service_1b00->characteristic(QBluetoothUuid(QString("F8011B03-F66E-494E-A672-A1E660829EAE")));
            if (!Char_1b03.isValid()) {
                qDebug("1b03 not found.");
                break;
            }
            m_1b03_notificationDesc = Char_1b03.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (m_1b03_notificationDesc.isValid())
                m_service_1b00->writeDescriptor(m_1b03_notificationDesc, QByteArray::fromHex("0100"));

            break;
        }
        default : break;
    }
}

/**
 * @brief Bluetooth::slot_1b00_updateValue
 * @param c
 * @param value
 */
void Bluetooth::slot_1b00_updateValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // K50 状态
    if (c.uuid() == QBluetoothUuid(QString("F8011B01-F66E-494E-A672-A1E660829EAE")))
    {
        if( value.length() != 2 )
        {
            qDebug("(error)K50状态长度错误!");
            return;
        }
        auto data = reinterpret_cast<const quint8 *>(value.constData());
        quint8 flags1 = *data;
        quint8 flags2 = *(data+1);

        qDebug("k50状态 = %d-%d", flags1, flags2);
    }

    // 预热剩余时间
    if (c.uuid() == QBluetoothUuid(QString("F8011B02-F66E-494E-A672-A1E660829EAE")))
    {
        if( value.length() != 2 )
        {
            qDebug("(error)预热剩余时间长度错误!");
            return;
        }
        auto data = reinterpret_cast<const quint8 *>(value.constData());
        quint8 flags1 = *data;

        qDebug("预热剩余时间 = %d", flags1);
    }

    // 呼吸数据包
    if (c.uuid() == QBluetoothUuid(QString("F8011B03-F66E-494E-A672-A1E660829EAE")))
    {
        if( value.length() != 56 )
        {
            qDebug("(error)呼吸数据包长度错误!");
            return;
        }
        auto data = reinterpret_cast<const quint8 *>(value.constData());

        qDebug("呼吸数据包 = ");
        for( int i=0; i<56; i++ )
            qDebug("%d ", data[i]);
    }

}
