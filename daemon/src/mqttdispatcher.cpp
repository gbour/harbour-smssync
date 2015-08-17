#include "mqttdispatcher.h"

#include <QDebug>
#include "qmqtt_client.h"

MqttDispatcher::MqttDispatcher(QString server, quint32 port, QObject *parent) :
    QObject(parent)
{
    this->client = new QMQTT::Client(server, port);
    QObject::connect(this->client, SIGNAL(connected()), this, SLOT(sendMessage()));

}

void MqttDispatcher::connect() {
    this->client->connect();
}

void MqttDispatcher::sendMessage() {
    qDebug() << "CONNECTED - send message";
    QMQTT::Message msg(34554, "/foo/bar", "plop");
    this->client->publish(msg);
}
