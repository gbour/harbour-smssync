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
#include <QSettings>
#include <QCoreApplication>

#include "mqttdispatcher.h"
#include "smslistener.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("harbour-smssyncd");
    app.setOrganizationName("gbour");

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
                     SIGNAL(SmsRecv(QString,QString,QString)),
                     &dispatcher,
                     SLOT(sendInMessage(QString,QString,QString)));

    QObject::connect(&watcher,
                     SIGNAL(SmsSent(QString,QString,QString)),
                     &dispatcher,
                     SLOT(sendOutMessage(QString,QString,QString)));

    QObject::connect(&watcher,
                     SIGNAL(SmsAcked(QString)),
                     &dispatcher,
                     SLOT(acknowledgement(QString)));

    return app.exec();
}

