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

#ifndef SMSLISTENER_H
#define SMSLISTENER_H

#include <QObject>

#include <TelepathyQt/Types>
#include <TelepathyQt/Account>
#include <TelepathyQt/SimpleTextObserver>
#include <TelepathyQt/Message>

#include <qt5/QtContacts/QContactManager>

#include "message.h"
#include "contact.h"

namespace Tp {
    class PendingOperation;
    class ReceivedMessage;
}

class SmsListener : public QObject
{
    Q_OBJECT
public:
    explicit SmsListener(QObject *parent = 0);

signals:
    void SmsRecv(Message *msg);
    void SmsSent(Message *msg);
    void SmsAcked(Message *msg);

public slots:

private slots:
    void onAccountReady(Tp::PendingOperation *op);
    void onReceivedMessage(const Tp::ReceivedMessage&, const Tp::TextChannelPtr&);
    void onSentMessage(const Tp::Message &msg, Tp::MessageSendingFlags flags,
                       const QString &token, const Tp::TextChannelPtr &chan);

private:
    Tp::AccountPtr account;
    Tp::SimpleTextObserverPtr observer;

    Tp::Contact *sender;
    QtContacts::QContactManager contactMgr;

    Contact* getContact(QString phoneNumber);
};

#endif // SMSLISTENER_H
