#include "mqttdispatcher.h"

#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>

#include "qmqtt/qmqtt_client.h"
#include "qmqtt/qmqtt_message.h"

MqttDispatcher::MqttDispatcher(QString deviceid, QString server, quint32 port,
                               int ping, int keepalive, QObject *parent) :
    QObject(parent)
{
    _msgid = 1;
    _deviceid  = deviceid;
    _ping      = ping * 1000; // store in ms
    _keepalive = keepalive;

    client = new QMQTT::Client(server, port);
    qDebug() << server << port << client;
    client->autoReconnect();

    pingTimer = new QTimer();
    QObject::connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
}

void MqttDispatcher::connect() {
    client->connect();
    qDebug() << client << client->clientId() << client->isConnected() << client->keepalive();

    // set a very long keepalive (10 minutes) but short ping (10 secs)
    // ping is slow down (a lot) when phone entering deep sleep mode (up to 1'30'')
    // but we wan't to keep socket opened as long as possible to be able to receive notifications
    client->setKeepAlive(_keepalive);
    pingTimer->start(_ping);
}


void MqttDispatcher::sendMessage(QString dir, QString id, QString from, QString content) {
    qDebug() << "is connected:" << client->isConnected();
    if ( !client->isConnected() ) {
        this->connect();
    };

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
    qDebug() << "is connected:" << this->client->isConnected();
    if ( !client->isConnected() ) {
        this->connect();
    }

    QJsonObject payload;
    payload["type"] = QString("ack");
    payload["id"]   = id;

    QJsonDocument jdoc(payload);
    QMQTT::Message msg(msgid(), QString("smssync/%1/notify").arg(_deviceid),
                    jdoc.toJson(QJsonDocument::Compact));

    client->publish(msg);

}

/*
 * we don't send ping nor reconnects when client is disconnected
 * connection will be reestablished when we'll send a message
 *
 */
void MqttDispatcher::ping() {
    if ( !client->isConnected() ) {
        qDebug() << "currently disconnect, stopping ping timer";

        pingTimer->stop();
        return;
    }

    qDebug() << "send ping" << QTime::currentTime();
    client->ping();
}

