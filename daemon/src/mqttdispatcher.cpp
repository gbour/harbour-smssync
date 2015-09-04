/*
    Copyright (C) 2015 Guillaume Bour.
    Contact: Guillaume Bour <buillaume@bour.cc>
    All rights reserved.

    This file is part of «harbour-smssync app».

    «harbour-smssync app» is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    «harbour-smssync app» is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with «harbour-smssync app».  If not, see <http://www.gnu.org/licenses/>.
*/

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

    client = new QMQTT::Client(server, port, QMQTT::Transport::SSL);
    qDebug() << server << port << client <<
                "(" << QMQTT::QTransportString(QMQTT::Transport::SSL) << "transport)";
    //client->autoReconnect();

    pingTimer = new QTimer();
    QObject::connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
    QObject::connect(client, SIGNAL(connacked(quint8)), this, SLOT(onConnected()));
}

void MqttDispatcher::connect() {
    // set a very long keepalive (10 minutes) but short ping (10 secs)
    // ping is slow down (a lot) when phone entering deep sleep mode (up to 1'30'')
    // but we wan't to keep socket opened as long as possible to be able to receive notifications
    client->setKeepAlive(_keepalive);
    //pingTimer->start(_ping);

    client->connect();
    qDebug() << client << client->clientId() << client->isConnected() << client->keepalive();
}

void MqttDispatcher::sendMessage(Message *msg) {
    // always enqueue message in order to always respect delivery order
    qDebug() << "enqueuing msg id" << msg->id();
    this->_msgqueue.append(msg);

    qDebug() << "is connected:" << client->isConnected();
    if ( !client->isConnected() ) {
        this->connect();
        return;
    };

    // sync send
    _deliver();
}

void MqttDispatcher::_deliver() {
    qDebug() << "delivering" << _msgqueue.length() << "messages";
    while( !_msgqueue.isEmpty() ) {
        _send(_msgqueue.takeFirst());
    }
}

inline void MqttDispatcher::_send(const Message *msg) {
    qDebug() << "sending msg" << msg->id();
    QString topic = QString("smssync/%1/notify").arg(_deviceid);

    QJsonDocument jdoc(*msg->toJSON());
    QMQTT::Message qmsg(msgid(), topic, jdoc.toJson(QJsonDocument::Compact));

    client->publish(qmsg);
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

