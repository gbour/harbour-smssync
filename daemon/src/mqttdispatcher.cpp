#include "mqttdispatcher.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "qmqtt/qmqtt_client.h"
#include "qmqtt/qmqtt_message.h"

MqttDispatcher::MqttDispatcher(QString deviceid, QString server, quint32 port, QObject *parent) :
    QObject(parent)
{
    _msgid = 1;
    _deviceid = deviceid;

    client = new QMQTT::Client(server, port);
    qDebug() << server << port << client;
    client->autoReconnect();

}

void MqttDispatcher::connect() {
    client->connect();
    qDebug() << client << client->clientId() << client->isConnected() << client->keepalive();

    client->setKeepAlive(120);
}

void MqttDispatcher::sendMessage(QString dir, QString id, QString from, QString content) {
    QJsonObject payload;
    payload["type"]    = QString("msg");
    payload["dir"]     = dir;
    payload["id"]      = id;
    payload["from"]    = from;
    payload["content"] = content;

    QJsonDocument jdoc(payload);
    QMQTT::Message msg(msgid(), QString("smssync/%1/notify").arg(_deviceid),
                       jdoc.toJson(QJsonDocument::Compact));

    client->publish(msg);
}

void MqttDispatcher::acknowledgement(QString id) {
    QJsonObject payload;
    payload["type"] = QString("ack");
    payload["id"]   = id;

    QJsonDocument jdoc(payload);
    QMQTT::Message msg(msgid(), QString("smssync/%1/notify").arg(_deviceid),
                    jdoc.toJson(QJsonDocument::Compact));

    client->publish(msg);

}
