#ifndef USER_INFO_H
#define USER_INFO_H

#include <QObject>
#include <QJsonObject>

class user_info : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName NOTIFY InfoChanged)
    Q_PROPERTY(QString weight READ getWeight NOTIFY InfoChanged)

public:
    explicit user_info(QJsonObject obj);

    QJsonObject json_obj;

    QString name;
    QString weight;

    QString getName() const;
    QString getWeight() const;

signals:
    void InfoChanged();

};

#endif // USER_INFO_H
