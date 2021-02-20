#ifndef USER_INFO_H
#define USER_INFO_H

#include <QObject>
#include <QJsonObject>

class user_info : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName NOTIFY InfoChanged)
    Q_PROPERTY(int age READ getAge NOTIFY InfoChanged)
    Q_PROPERTY(double height READ getHeight NOTIFY InfoChanged)
    Q_PROPERTY(double weight READ getWeight NOTIFY InfoChanged)

public:
    explicit user_info();
    explicit user_info(QString nmae, int age, double height, double weight);

    QString name;       // 名字
    int age;            // 年龄
    float height;      // 身高
    float weight;      // 体重

    QString getName() const;
    int getAge() const;
    float getHeight() const;
    float getWeight() const;

signals:
    void InfoChanged();

};

struct user_info_t
{
    QString name;       // 名字
    int age;            // 年龄
    float height;      // 身高
    float weight;      // 体重
};

Q_DECLARE_METATYPE(struct user_info_t)

QDataStream &operator<<(QDataStream &out, const struct user_info_t info);
QDataStream &operator>>(QDataStream &out, struct user_info_t &info);

#endif // USER_INFO_H
