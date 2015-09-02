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

#include <signal.h>
#include <QDebug>
#include <QObject>
#include <QSettings>
#include <QCoreApplication>

#include "mqttdispatcher.h"
#include "smslistener.h"

#include "message.h"

#ifdef QT_DEBUG

class Unittest : public QObject {
    Q_OBJECT

public:
    MqttDispatcher *m_dispatcher;
    QTimer *m_timer;
    int m_i;

    Unittest(MqttDispatcher *dispatcher) : QObject(0) {
        qDebug() << "starting unittest";
        m_dispatcher = dispatcher;
        m_timer      = new QTimer();
        m_i          = 1;

        QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(onTick()));
        m_timer->start(1000);
    }


public slots:
    void onTick() {
        qDebug() << "timer fired";

        Message *msg = new Message(MessageType::INCOMING, QString::number(m_i++));

        Contact *c = new Contact("+33600000001");
        c->setName("John Doe");
        msg->setContact(c);
        msg->setContent(QString("Hi Pal! %1").arg(QString::number(qrand())));
        m_dispatcher->sendMessage(msg);
    }


};

#include "daemon.moc"

enum ENV {
    UNKNOWN = 0,
    EMULATOR,
    J1PHONE,
    J1TABLET
};

const QString _qEnvString[] { "UNKNOWN", "EMULATOR", "J1PHONE", "J1TABLET" };


ENV env() {
    QFile file("/etc/meego-release");
    if(!file.open(QIODevice::ReadOnly)) {
        return ENV::UNKNOWN;
    }

    QTextStream in(&file);
    QString rel = in.readAll().toLower();
    file.close();

    if(rel.contains("emulator")) {
        return ENV::EMULATOR;
    }

    if(rel.contains("-armv7hl")) {
        return ENV::J1PHONE;
    }

    return ENV::UNKNOWN;
};

#endif

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("harbour-smssyncd");
    app.setOrganizationName("gbour");

#ifdef QT_DEBUG
    ENV curenv = env();
    qDebug() << "RUNNING ENV:" << _qEnvString[curenv];
#endif

    // does not work in debug mode, where application & configuration are
    // deployed in /opt/sdk/harbour-smssync
    QSettings settings(QSettings::SystemScope, "harbour-smssync", "harbour-smssyncd");
    settings.beginGroup("mqtt");

    qDebug() << argv[0] << "0.1" << app.applicationDirPath();
    qDebug() << "STARTING. mqtt server = "
             << settings.value("server").toString() << ":"
             << settings.value("port").toInt();

    //MQTT client
    MqttDispatcher dispatcher(settings.value("deviceid").toString(),
                              settings.value("server").toString(),
                              settings.value("port").toInt(),
                              settings.value("ping").toInt(),
                              settings.value("keepalive").toInt());
    dispatcher.connect();
    settings.endGroup();


    // SMS listener
    SmsListener watcher;
    Q_UNUSED(watcher);

    QObject::connect(&watcher,
                     SIGNAL(SmsRecv(Message*)),
                     &dispatcher,
                     SLOT(sendMessage(Message*)));

    QObject::connect(&watcher,
                     SIGNAL(SmsSent(Message*)),
                     &dispatcher,
                     SLOT(sendMessage(Message*)));

    QObject::connect(&watcher,
                     SIGNAL(SmsAcked(Message*)),
                     &dispatcher,
                     SLOT(sendMessage(Message*)));


#ifdef QT_DEBUG
    // TESTING ONLY
    Unittest *test;
    Q_UNUSED(test);

    if(curenv == ENV::EMULATOR) {
        test = new Unittest(&dispatcher);
    }
#endif

    return app.exec();
}

