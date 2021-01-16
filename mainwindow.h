#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQmlApplicationEngine>

class mainwindow : public QObject
{
    Q_OBJECT
public:
    explicit mainwindow(QObject *parent = nullptr);

    QQmlApplicationEngine *g_qmlEngine;
    QObject *g_rootObject;

signals:

};

#endif // MAINWINDOW_H
