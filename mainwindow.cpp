#include "mainwindow.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QSettings>
#include <QQuickStyle>
#include "bluetooth.h"
#include "jsonhandle.h"
#include "user_info.h"

mainwindow::mainwindow(QObject *parent) : QObject(parent)
{
    // 主题设置
    QSettings settings;
    QString style = QQuickStyle::name();
    if (!style.isEmpty())
        settings.setValue("style", style);
    else
        QQuickStyle::setStyle(settings.value("style").toString());

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
}

QVariant mainwindow::get_user_info_list()
{
    qDebug() << "run get_user_info_list";
    return QVariant::fromValue(m_user_info);
}

bool mainwindow::add_user(QString name, int age, double height, double weight)
{
    if( user_info_settings->contains(name) == true )
        return false;

    struct user_info_t info = { name, age, height, weight };
    m_user_info.append(new user_info(info.name, info.age, info.height, info.weight));
    emit userInfoChanged();

    user_info_settings->setValue(name, QVariant::fromValue(info));
    return true;
}

int index = 0;
void mainwindow::button_test()
{
    qDebug("button_test()");

}
