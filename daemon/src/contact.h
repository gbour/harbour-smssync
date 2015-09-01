#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

class Contact : public QObject
{
    Q_OBJECT
public:
    explicit Contact(QString phoneNumber, QObject *parent = 0);
    QJsonObject* toJSON() const;

    QString name() const;
    void setName(const QString &name);

signals:

public slots:

private:
    QString m_phoneNumber;
    QString m_name;
    //TODO:avatar
};

#endif // CONTACT_H
