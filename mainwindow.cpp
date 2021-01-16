#include "mainwindow.h"
#include <QQmlContext>
#include <QQmlComponent>
#include <QSettings>
#include <QQuickStyle>
#include "bluetooth.h"

mainwindow::mainwindow(QObject *parent) : QObject(parent)
{
    QSettings settings;
    QString style = QQuickStyle::name();
    if (!style.isEmpty())
        settings.setValue("style", style);
    else
        QQuickStyle::setStyle(settings.value("style").toString());

    g_qmlEngine = new QQmlApplicationEngine(this);
    QQmlComponent lv_component(g_qmlEngine, QUrl(QStringLiteral("qrc:/main.qml")));
    g_rootObject = lv_component.create();
    g_rootObject->setParent(this);

    Bluetooth * buletooth = new Bluetooth(g_rootObject);
    g_qmlEngine->rootContext()->setContextProperty("mainwindow", this);
    g_qmlEngine->rootContext()->setContextProperty("buletooth", buletooth);
    g_qmlEngine->rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());
}

int index = 0;
void mainwindow::button_test()
{
    qDebug("button_test()");

    g_rootObject->setProperty("gv_name",index);
    index ++;
}
