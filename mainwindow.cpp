#include "mainwindow.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QSettings>
#include <QQuickStyle>
#include "bluetooth.h"
#include "user_info.h"
#include <QDateTime>
#include <QMetaObject>

mainwindow::mainwindow(QObject *parent) : QObject(parent)
{
    // 主题设置
    sys_settings = new QSettings();
    QString style = QQuickStyle::name();
    if (!style.isEmpty())
        sys_settings->setValue("style", style);
    else
    {
        sys_settings->setValue("style", "Material");
        QQuickStyle::setStyle(sys_settings->value("style").toString());
    }

    g_qmlEngine = new QQmlApplicationEngine(this);
    g_qmlEngine->rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());
    g_qmlEngine->rootContext()->setContextProperty("mainwindow", this);
    QQmlComponent lv_component(g_qmlEngine, QUrl(QStringLiteral("qrc:/main.qml")));
    g_rootObject = lv_component.create();
    g_rootObject->setParent(this);

    buletooth = new Bluetooth(g_rootObject, &active_user);
    g_qmlEngine->rootContext()->setContextProperty("buletooth", buletooth);

    qRegisterMetaTypeStreamOperators<struct user_info_t >("struct user_info_t");

    user_info_settings = new QSettings("stuff.ini", QSettings::IniFormat);
    QStringList keyList = user_info_settings->allKeys();
    for(int i = 0; i < keyList.length(); i++)
    {
        QVariant variant = user_info_settings->value(keyList.at(i));
        struct user_info_t info = variant.value<struct user_info_t>();
        m_user_info.append(new user_info(info.name, info.age, info.height, info.weight));
    }
    emit userInfoChanged();

    if( sys_settings->contains("current_user_name") == true )
    {
        int curent_index = sys_settings->value("current_user_name").toInt();
        if( m_user_info.size() > curent_index )
        {
            active_user = (user_info *)m_user_info.at(curent_index);
            if( active_user != nullptr )
                g_rootObject->setProperty("current_user_name", active_user->name);

        }
    }
    else
    {
        g_rootObject->setProperty("current_user_name", tr("未指定"));
    }

    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    int time_int = time.toTime_t();
    if( time_int > 1617163200 ) // 2021-03-31 12:00:00
    {
        QVariant msg;
        QMetaObject::invokeMethod(g_rootObject, "permission", Q_ARG(QVariant, msg));
    }

    QString dateTime;
    dateTime += __DATE__;
    dateTime += __TIME__;
    dateTime.replace("  "," 0");
    QDateTime datatime = QLocale(QLocale::English).toDateTime(dateTime,"MMM dd yyyyhh:mm:ss");
    QString versions_str = datatime.toString("yyyyMMddhhmm");
    qDebug() << "versions_str = " << versions_str;
    QMetaObject::invokeMethod(g_rootObject, "get_versions_str", Q_ARG(QVariant, versions_str));
}

/**
 * @brief mainwindow::get_user_info_list
 * @return
 */
QVariant mainwindow::get_user_info_list()
{
    qDebug() << "run get_user_info_list";
    return QVariant::fromValue(m_user_info);
}

/**
 * @brief mainwindow::add_user  添加用户
 * @param name
 * @param age
 * @param height
 * @param weight
 * @return
 */
bool mainwindow::add_user(QString name, int age, float height, float weight)
{
    if( name.isEmpty() )
        return false;

    if( user_info_settings->contains(name) == true )
        return false;

    struct user_info_t info = { name, age, height, weight };
    m_user_info.append(new user_info(info.name, info.age, info.height, info.weight));
    emit userInfoChanged();

    user_info_settings->setValue(name, QVariant::fromValue(info));
    return true;
}

/**
 * @brief mainwindow::delete_uesr   删除用户
 * @param index
 */
bool mainwindow::delete_user(int index)
{
//    qDebug() << "delete_uesr index = " << index;
    if( m_user_info.empty() )
        return false;

    user_info *info = (user_info *)m_user_info.at(index);
    if( info != nullptr )
    {
        qDebug() << "delete_uesr name = " << info->name;
        if( info->name == active_user->name )
        {
            active_user = nullptr;
            g_rootObject->setProperty("current_user_name", tr("未指定"));
        }
        user_info_settings->remove(info->name);
        m_user_info.removeAt(index);
        emit userInfoChanged();
        return true;
    }
    return false;
}

/**
 * @brief mainwindow::switch_user   切换用户
 * @param index
 * @return
 */
bool mainwindow::switch_user(int index)
{
    if( m_user_info.empty() )
        return false;

    active_user = (user_info *)m_user_info.at(index);
    if( active_user != nullptr )
    {
        g_rootObject->setProperty("current_user_name", active_user->name);
        sys_settings->setValue("current_user_name", index);
        return true;
    }
    return false;
}


QByteArray intToByte(int i)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar) (0x000000ff & i);
    abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
    return abyte0;
}

int index = 9;
void mainwindow::button_test()
{
    qDebug("button_test()");
    QByteArray ch = intToByte(index);
    buletooth->m_service_1a00->writeCharacteristic(buletooth->Char_1a03, ch, QLowEnergyService::WriteWithResponse);
}

void mainwindow::set_wristband_addr(QString addr)
{
    qDebug() << "设置手环地址：" << addr;
    QByteArray abyte;
    QString zero("0");
    abyte.append(zero.toInt(nullptr, 16));
    QStringList list = addr.split(":");
    if( list.size() == 6 )
    {
        for (int i = list.size()-1; i >=0; i--)
        {
            QString current = list.at(i);
            qDebug() << "addr:" << current.toInt(nullptr, 16);
            abyte.append(current.toInt(nullptr, 16));
        }
        qDebug() << "abyte size:" << abyte.length();
        buletooth->m_service_1a00->writeCharacteristic(buletooth->Char_1a01, abyte, QLowEnergyService::WriteWithResponse);
    }
}
