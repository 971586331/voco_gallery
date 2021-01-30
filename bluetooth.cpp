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
                SLOT(addBlueToothDevicesToList(QBluetoothDeviceInfo)));
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
 * @brief Bluetooth::connect_device 连接到设备，供UI调用
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

/**
 * @brief Bluetooth::ble_connect_device 连接设备
 * @param info
 */
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
        g_rootObject->setProperty("scan_state", QString("已经连接到:").append(currentDeviceInfo.name()));
        g_rootObject->setProperty("connect_status", QString("已经连接到:").append(currentDeviceInfo.name()));
        g_rootObject->setProperty("led_color", "green");
        m_control->discoverServices();
    });

    connect(m_control, &QLowEnergyController::disconnected, this, [this]()
    {
        qDebug("LowEnergy控制器断开连接");
        g_rootObject->setProperty("scan_state", tr("连接断开，请点击按钮开始寻找设备并连接！"));
        g_rootObject->setProperty("connect_status", tr("未连接设备"));
        g_rootObject->setProperty("led_color", "black");
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
            m_service_1800 = m_control->createServiceObject(QBluetoothUuid(QBluetoothUuid::GenericAccess), this);

            if (m_service_1800) {
                connect(m_service_1800, &QLowEnergyService::stateChanged, this, &Bluetooth::slot_1800_serviceStateChanged);
                connect(m_service_1800, &QLowEnergyService::characteristicRead, this, &Bluetooth::slot_1800_characteristicRead);
                m_service_1800->discoverDetails();
            } else {
                qDebug("\"通用访问配置\"服务没有找到！");
            }
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
        if( gatt == QBluetoothUuid(QString(UUID_1900)) )
        {
            qDebug("服务：手环设备信息");
            m_service_1900 = m_control->createServiceObject(QBluetoothUuid(QString(UUID_1900)), this);
            if (m_service_1900) {
                connect(m_service_1900, &QLowEnergyService::stateChanged, this, &Bluetooth::slot_1900_serviceStateChanged);
                connect(m_service_1900, &QLowEnergyService::characteristicRead, this, &Bluetooth::slot_1900_characteristicRead);
                m_service_1900->discoverDetails();
            } else {
                qDebug("1A00服务没有找到！");
            }

        }
        if( gatt == QBluetoothUuid(QString(UUID_1A00)) )
        {
            qDebug("服务：下发K50数据");
            m_service_1a00 = m_control->createServiceObject(QBluetoothUuid(QString(UUID_1A00)), this);
            if (m_service_1a00) {
                connect(m_service_1a00, &QLowEnergyService::stateChanged, this, &Bluetooth::slot_1a00_serviceStateChanged);
                m_service_1a00->discoverDetails();
            } else {
                qDebug("1A00服务没有找到！");
            }
        }
        if( gatt == QBluetoothUuid(QString(UUID_1B00)) )
        {
            qDebug("服务：获得K50数据");
            m_service_1b00 = m_control->createServiceObject(QBluetoothUuid(QString(UUID_1B00)), this);
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
 * @brief Bluetooth::slot_1900_serviceStateChanged  手环设备信息
 * @param s
 */
void Bluetooth::slot_1900_serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
        case QLowEnergyService::DiscoveringServices:
            qDebug("1900 Discovering services...");
            break;
        case QLowEnergyService::ServiceDiscovered:
        {
            qDebug("1900 Service discovered.");

            Char_1901 = m_service_1900->characteristic(QBluetoothUuid(QString(UUID_1901)));
            if (!Char_1a01.isValid())
            {
                qDebug("(error)0x1901 not found.");
            }
            Char_1902 = m_service_1900->characteristic(QBluetoothUuid(QString(UUID_1902)));
            if (!Char_1902.isValid())
            {
                qDebug("(error)0x1902 not found.");
            }
            Char_1903 = m_service_1900->characteristic(QBluetoothUuid(QString(UUID_1903)));
            if (!Char_1903.isValid())
            {
                qDebug("(error)0x1903 not found.");
            }
            Char_1908 = m_service_1900->characteristic(QBluetoothUuid(QString(UUID_1908)));
            if (!Char_1908.isValid())
            {
                qDebug("(error)0x1908 not found.");
            }
            m_service_1900->readCharacteristic(Char_1908);

            break;
        }
        default : break;
    }
}

/**
 * @brief Bluetooth::slot_1900_characteristicRead 手环设备信息
 * @param characteristic
 * @param value
 */
void Bluetooth::slot_1900_characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if(characteristic.uuid() == QBluetoothUuid(QBluetoothUuid(QString(UUID_1901))))
    {
        qDebug() << "手环生产厂商 = " << value;
    }
    if(characteristic.uuid() == QBluetoothUuid(QBluetoothUuid(QString(UUID_1902))))
    {
        qDebug() << "手环产品型号 = " << value;
    }
    if(characteristic.uuid() == QBluetoothUuid(QBluetoothUuid(QString(UUID_1903))))
    {
        qDebug() << "手环产品序列号 = " << value;
    }
    if(characteristic.uuid() == QBluetoothUuid(QBluetoothUuid(QString(UUID_1908))))
    {
        if( value.length() == 7 )
        {
            const char *ch;
            ch = value.data();
            QString str1 = QString("%1:%2:%3:%4:%5:%6").arg(ch[6],2,16,QLatin1Char('0')).arg(ch[5],2,16,QLatin1Char('0')) \
                    .arg(ch[4],2,16,QLatin1Char('0')).arg(ch[3],2,16,QLatin1Char('0')).arg(ch[2],2,16,QLatin1Char('0')) \
                    .arg(ch[1],2,16,QLatin1Char('0'));
            qDebug() << "Flash中保存的手环地址 = " << str1.toUpper();
            g_rootObject->setProperty("current_wristband_addr", str1.toUpper());
        }
    }
}

/**
 * @brief Bluetooth::slot_1a00_serviceStateChanged 下发K50数据
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

            Char_1a01 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A01)));
            if (!Char_1a01.isValid())
            {
                qDebug("(error)0x1a01 not found.");
            }
            Char_1a02 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A02)));
            if (!Char_1a02.isValid())
            {
                qDebug("(error)0x1a02 not found.");
            }
            Char_1a03 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A03)));
            if (!Char_1a03.isValid())
            {
                qDebug("(error)0x1a03 not found.");
            }
            Char_1a04 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A04)));
            if (!Char_1a04.isValid())
            {
                qDebug("(error)0x1a04 not found.");
            }
            Char_1a05 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A05)));
            if (!Char_1a05.isValid())
            {
                qDebug("(error)0x1a05 not found.");
            }
            Char_1a06 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A06)));
            if (!Char_1a06.isValid())
            {
                qDebug("(error)0x1a06 not found.");
            }
            Char_1a07 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A07)));
            if (!Char_1a07.isValid())
            {
                qDebug("(error)0x1a07 not found.");
            }
            Char_1a08 = m_service_1a00->characteristic(QBluetoothUuid(QString(UUID_1A08)));
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
 * @brief Bluetooth::slot_1b00_serviceStateChanged 获得k50数据
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

            QLowEnergyCharacteristic Char_1b01 = m_service_1b00->characteristic(QBluetoothUuid(QString(UUID_1B01)));
            if (!Char_1b01.isValid()) {
                qDebug("1b01 not found.");
                break;
            }
            m_1b01_notificationDesc = Char_1b01.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (m_1b01_notificationDesc.isValid())
                m_service_1b00->writeDescriptor(m_1b01_notificationDesc, QByteArray::fromHex("0100"));

            QLowEnergyCharacteristic Char_1b02 = m_service_1b00->characteristic(QBluetoothUuid(QString(UUID_1B02)));
            if (!Char_1b02.isValid()) {
                qDebug("1b02 Data not found.");
                break;
            }
            m_1b02_notificationDesc = Char_1b02.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (m_1b02_notificationDesc.isValid())
                m_service_1b00->writeDescriptor(m_1b02_notificationDesc, QByteArray::fromHex("0100"));

            QLowEnergyCharacteristic Char_1b03 = m_service_1b00->characteristic(QBluetoothUuid(QString(UUID_1B03)));
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
 * @brief Bluetooth::slot_1b00_updateValue 获得k50数据
 * @param c
 * @param value
 */
void Bluetooth::slot_1b00_updateValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // K50 状态
    if (c.uuid() == QBluetoothUuid(QString(UUID_1B01)))
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
    if (c.uuid() == QBluetoothUuid(QString(UUID_1B02)))
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
    if (c.uuid() == QBluetoothUuid(QString(UUID_1B03)))
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

/**
 * @brief Bluetooth::slot_1800_serviceStateChanged 通用访问配置
 * @param s
 */
void Bluetooth::slot_1800_serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
        case QLowEnergyService::DiscoveringServices:
            qDebug("1b00 Discovering services...");
            break;
        case QLowEnergyService::ServiceDiscovered:
        {
            qDebug("1b00 Service discovered.");

            Char_2a00 = m_service_1800->characteristic(QBluetoothUuid::DeviceName);
            if (!Char_2a00.isValid()) {
                qDebug("2a00 not found.");
                break;
            }
            m_service_1800->readCharacteristic(Char_2a00);

            break;
        }
        default : break;
    }
}

/**
 * @brief Bluetooth::slot_1800_characteristicRead 通用访问配置
 * @param characteristic
 * @param value
 */
void Bluetooth::slot_1800_characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if(characteristic.uuid() == QBluetoothUuid(QBluetoothUuid::DeviceName))
    {
        qDebug() << "DeviceName = " << value;
    }
}
