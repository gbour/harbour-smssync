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
    void sendMessage();
private:
    QMQTT::Client *client;

};

#endif // MQTTDISPATCHER_H
