#include "user_info.h"
#include <QDebug>

user_info::user_info()
{

}

user_info::user_info(QString nmae, int age, double height, double weight) :
    name(nmae), age(age), height(height), weight(weight)
{
}

QString user_info::getName() const
{
    return name;
}

int user_info::getAge() const
{
    return age;
}

double user_info::getHeight() const
{
    return height;
}

double user_info::getWeight() const
{
    return weight;
}


//定义怎么把自定义结构体的数据写入一条qsettings
QDataStream &operator<<(QDataStream &out, const struct user_info_t info)
{

    out << info.name << QString::number(info.age) \
        << QString::number(info.height, 10, 4) \
        << QString::number(info.weight, 10, 4);

    return out;
}

//定义怎么把从qsettings读出的一条数据保存到自定义结构体中
QDataStream &operator>>(QDataStream &out, struct user_info_t& info)
{
    QString str1;
    out >> str1;
    info.name = str1;

    QString str2;
    out >> str2;
    info.age = str2.toInt();

    QString str3;
    out >> str3;
    info.height = str3.toDouble();

    QString str4;
    out >> str4;
    info.weight = str4.toDouble();

    return out;
}
