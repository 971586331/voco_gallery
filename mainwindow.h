#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "bluetooth.h"
#include "jsonhandle.h"
#include <QSettings>
#include "user_info.h"

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

    user_info *active_user;
    QSettings *sys_settings;
    QSettings *user_info_settings;
    QList<QObject *> m_user_info;
    QVariant get_user_info_list();
    Q_INVOKABLE bool add_user(QString nmae, int age, double height, double weight);
    Q_INVOKABLE bool delete_user(int index);
    Q_INVOKABLE bool switch_user(int index);

    Q_INVOKABLE void button_test();

signals:
    void userInfoChanged();

};

#endif // MAINWINDOW_H
