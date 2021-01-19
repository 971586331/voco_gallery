#ifndef JSONHANDLE_H
#define JSONHANDLE_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

class JsonHandle : public QObject
{
    Q_OBJECT
public:
    explicit JsonHandle(QString path = "");

    QFile *file;                    //配置文件的句柄
    QJsonObject rootObj;            //配置文件的主节点

signals:

public slots:

};

#endif // JSONHANDLE_H
