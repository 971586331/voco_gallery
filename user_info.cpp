#include "user_info.h"
#include <QJsonObject>
#include <QDebug>

user_info::user_info(QJsonObject obj) : json_obj(obj)
{
    name = json_obj.value("name").toString();
    weight = json_obj.value("weight").toString();

    qDebug() << "name = "<< name;
    qDebug() << "weight = "<< weight;
}

QString user_info::getName() const
{
    return name;
}

QString user_info::getWeight() const
{
    return weight;
}
