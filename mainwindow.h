#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "bluetooth.h"
#include "jsonhandle.h"

class mainwindow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant user_info READ get_user_info_list NOTIFY userInfoChanged)

public:
    explicit mainwindow(QObject *parent = nullptr);

    QQmlApplicationEngine *g_qmlEngine;
    QObject *g_rootObject;

    Bluetooth *buletooth;
    JsonHandle *user_info_obj;

    QList<QObject*> m_user_info;
    QVariant get_user_info_list();

    Q_INVOKABLE void button_test();

signals:
    void userInfoChanged();

};

#endif // MAINWINDOW_H
