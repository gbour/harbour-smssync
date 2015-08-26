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
    void sendInMessage(QString id, QString from, QString message) {
        sendMessage("in", id, from, message);
    };

    void sendOutMessage(QString id, QString from, QString message) {
        sendMessage("out", id, from, message);
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

    void sendMessage(QString dir, QString id, QString from, QString message);
    void _send(QString topic, QJsonObject payload);
    void _deliver();

    quint16 msgid() {
        return(_msgid++);
    }
};

#endif // MQTTDISPATCHER_H
