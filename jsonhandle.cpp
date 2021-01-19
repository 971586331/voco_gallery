#include "jsonhandle.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

JsonHandle::JsonHandle(QString path)
{
    //打开配置文件
    file = new QFile(path);
    if(!file->open(QIODevice::ReadOnly))
    {
        qDebug() << path << "文件打开失败！" << file->errorString();
        return;
    }
    //读出json文档
    QJsonDocument jdc(QJsonDocument::fromJson(file->readAll()));
    file->close();
    rootObj = jdc.object();
    if( rootObj.isEmpty() == true )
    {
        qDebug() << path << "文件主节点为空！";
        jdc.setObject(rootObj);
        return;
    }
}

