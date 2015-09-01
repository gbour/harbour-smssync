#include "message.h"

Message::Message(MessageType type, QString id, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_id   = id;
}

QJsonObject *Message::toJSON() const
{
    QJsonObject *obj = new QJsonObject();
    obj->insert("id", m_id);

    if (m_type == MessageType::ACKNOWLEDGEMENT) {
        obj->insert("type", QString("ack"));
        return obj;
    }

    obj->insert("type", QString("msg"));
    obj->insert("dir" , QString(m_type == MessageType::INCOMING ? "in" : "out"));

    obj->insert("content", m_content);
    obj->insert("contact", *m_contact->toJSON());

    return obj;
}

QString Message::content() const
{
    return m_content;
}

void Message::setContent(const QString &content)
{
    m_content = content;
}

Contact* Message::contact()
{
    return m_contact;
}

void Message::setContact(Contact* contact)
{
    m_contact = contact;
}
QString Message::id() const
{
    return m_id;
}

void Message::setId(const QString &id)
{
    m_id = id;
}




