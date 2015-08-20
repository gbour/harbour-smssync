#ifndef MQTTDISPATCHER_H
#define MQTTDISPATCHER_H

#include <QObject>
#include "qmqtt_client.h"

class MqttDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MqttDispatcher(QString server, quint32 port = 1883, QObject *parent = 0);

signals:

public slots:
    void connect();
    void sendInMessage(QString id, QString from, QString message) {
        sendMessage("in", id, from, message);
    };

    void sendOutMessage(QString id, QString from, QString message) {
        sendMessage("out", id, from, message);
    };

    void acknowledgement(QString id);

private:
    quint16 _msgid;
    QMQTT::Client *client;

    void sendMessage(QString dir, QString id, QString from, QString message);
    quint16 msgid() {
        return(_msgid++);
    }
};

#endif // MQTTDISPATCHER_H
