
#include <QJsonObject>
#include "contact.h"


Contact::Contact(QString phoneNumber, QObject *parent) :
    QObject(parent)
{
    m_phoneNumber = phoneNumber;
    m_name        = QString();
}

QJsonObject *Contact::toJSON() const
{
    QJsonObject *obj = new QJsonObject();

    obj->insert("phoneNumber", m_phoneNumber);
    obj->insert("name"       , m_name.isNull() ? QJsonValue() : m_name);

    return obj;
}

QString Contact::name() const
{
    return m_name;
}

void Contact::setName(const QString &name)
{
    m_name = name;
}

