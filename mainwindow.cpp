#include "mainwindow.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QSettings>
#include <QQuickStyle>
#include "bluetooth.h"
#include "user_info.h"

mainwindow::mainwindow(QObject *parent) : QObject(parent)
{
    // 主题设置
    sys_settings = new QSettings();
    QString style = QQuickStyle::name();
    if (!style.isEmpty())
        sys_settings->setValue("style", style);
    else
        QQuickStyle::setStyle(sys_settings->value("style").toString());

    g_qmlEngine = new QQmlApplicationEngine(this);
    g_qmlEngine->rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());
    g_qmlEngine->rootContext()->setContextProperty("mainwindow", this);
    QQmlComponent lv_component(g_qmlEngine, QUrl(QStringLiteral("qrc:/main.qml")));
    g_rootObject = lv_component.create();
    g_rootObject->setParent(this);

    buletooth = new Bluetooth(g_rootObject);
    g_qmlEngine->rootContext()->setContextProperty("buletooth", buletooth);

    // 用户信息
//    user_info_obj = new JsonHandle(":/user_info.json");
//    QJsonArray array = user_info_obj->rootObj.value("user_info").toArray();
//    for(int i = 0; i< array.count(); i++)
//    {
//        QJsonObject name_obj = array.at(i).toObject();
//        m_user_info.append(new user_info(name_obj));
//        emit userInfoChanged();
//    }

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
bool mainwindow::add_user(QString name, int age, double height, double weight)
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
