#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QJsonObject>
#include "contact.h"

// not the right denomination
enum MessageType {
    INCOMING = 0,
    OUTGOING,
    ACKNOWLEDGEMENT
};

const QString _qMessageString[] { "INCOMING", "OUTGOING", "ACKNOWLEDGEMENT" };
#define QMessageString(t) _qMessageString[t]

class Message : public QObject
{
    Q_OBJECT
public:    
    explicit Message(MessageType type, QString id, QObject *parent = 0);
    QJsonObject *toJSON() const;

    QString content() const;
    void setContent(const QString &content);

    Contact* contact();
    void setContact(Contact* contact);

    QString id() const;
    void setId(const QString &id);

signals:

public slots:

private:
    MessageType m_type;
    QString     m_id;
    Contact*    m_contact;

    // text message
    QString     m_content;
};

#endif // MESSAGE_H
