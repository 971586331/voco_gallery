#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "bluetooth.h"

class mainwindow : public QObject
{
    Q_OBJECT
public:
    explicit mainwindow(QObject *parent = nullptr);

    QQmlApplicationEngine *g_qmlEngine;
    QObject *g_rootObject;

    Bluetooth * buletooth;

    Q_INVOKABLE void button_test();

signals:

};

#endif // MAINWINDOW_H
