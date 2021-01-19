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
    user_info_obj = new JsonHandle(":/user_info.json");
    QJsonArray array = user_info_obj->rootObj.value("user_info").toArray();
    for(int i = 0; i< array.count(); i++)
    {
        QJsonObject name_obj = array.at(i).toObject();
        m_user_info.append(new user_info(name_obj));
        emit userInfoChanged();
    }

}

QVariant mainwindow::get_user_info_list()
{
    qDebug() << "run get_user_info_list";
    return QVariant::fromValue(m_user_info);
}

int index = 0;
void mainwindow::button_test()
{
    qDebug("button_test()");

    m_user_info.clear();
    QJsonArray array = user_info_obj->rootObj.value("user_info").toArray();
    for(int i = 0; i< array.count(); i++)
    {
        QJsonObject name_obj = array.at(i).toObject();
        m_user_info.append(new user_info(name_obj));
        emit userInfoChanged();
    }
}
