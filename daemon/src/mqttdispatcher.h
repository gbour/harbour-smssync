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

#ifndef MQTTDISPATCHER_H
#define MQTTDISPATCHER_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>

#include "qmqtt_client.h"

class MqttDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MqttDispatcher(QString deviceid, QString server, quint32 port = 1883,
                            int ping = 60, int keepalive = 60, QObject *parent = 0);

signals:

public slots:
    void connect();
    void sendInMessage(QString id, QString cn, QString from, QString message) {
        sendMessage("in", id, cn, from, message);
    };

    void sendOutMessage(QString id, QString cn, QString from, QString message) {
        sendMessage("out", id, cn, from, message);
    };

    void acknowledgement(QString id);
    void ping();

private slots:
    void onConnected() {
        qDebug() << "mqtt connection established";
        _deliver();
    }

private:
    quint16 _msgid;
    QString _deviceid;
    int     _ping;
    int     _keepalive;

    QList<QPair<QString, QJsonObject>> _msgqueue;

    QMQTT::Client *client;
    QTimer *pingTimer;

    void sendMessage(QString dir, QString id, QString contactName, QString contactPhoneNumber, QString message);
    void _send(QString topic, QJsonObject payload);
    void _deliver();

    quint16 msgid() {
        return(_msgid++);
    }
};

#endif // MQTTDISPATCHER_H
