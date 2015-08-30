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

#include <QDebug>

#include <TelepathyQt/Debug>
#include <TelepathyQt/Constants>
#include <TelepathyQt/Types>

#include <TelepathyQt/TextChannel>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/ReceivedMessage>

// not autocompletion using this path
//#include <Qt5Contacts/QContact>
#include <qt5/QtContacts/QContact>
#include <qt5/QtContacts/QContactManager>
#include <qt5/QtContacts/QContactDetail>
#include <qt5/QtContacts/QContactPhoneNumber>
#include <qt5/QtContacts/QContactDetailFilter>

using namespace QtContacts;

#include "smslistener.h"

SmsListener::SmsListener(QObject *parent) :
    QObject(parent)
{
    qDebug() << "starting SMS listener...";

    Tp::registerTypes();
    Tp::enableDebug(true);
    Tp::enableWarnings(true);



    // is account name always the same ??
    account = Tp::Account::create(TP_QT_ACCOUNT_MANAGER_BUS_NAME,
                                  "/org/freedesktop/Telepathy/Account/ring/tel/account0");

    connect(account->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountReady(Tp::PendingOperation *)));


    // initialize contacts manager
    QContactManager contactMgr("org.nemomobile.contacts.sqlite");
    qDebug() << contactMgr.managerName();
}

/*
 * Callback - when account handler is initialized
 */
void SmsListener::onAccountReady(Tp::PendingOperation *op) {
    Q_UNUSED(op);

    if(!account->isReady() || !account->isValid()) {
        qDebug() << "account is not ready/valid";
        return;
    }

    qDebug() << "account " << account->uniqueIdentifier() << " initialized";

    // initializing connection
    Tp::ConnectionPtr conn = account->connection();
    // wait for the connection to becomes ready
    if( !( conn->isReady() && conn->isValid() ) ) {
        QEventLoop loop;
        QObject::connect(
                    conn.data()->becomeReady(),
                    &Tp::PendingOperation::finished,
                    &loop,
                    &QEventLoop::quit);
        loop.exec();

        if (!conn->isReady() || !conn->isValid()) {
            qDebug() << "connection is not ready/valid";
            return;
        }
    }
    qDebug() << "connection " << conn->cmName() << " is ready";

    // monitoring received/sent messages (SMS)
    observer = Tp::SimpleTextObserver::create(account);
    connect(observer.data(),
            SIGNAL(messageReceived(Tp::ReceivedMessage, Tp::TextChannelPtr)),
            this,
            SLOT(onReceivedMessage(Tp::ReceivedMessage,Tp::TextChannelPtr)));

    connect(observer.data(),
            SIGNAL(messageSent(Tp::Message,Tp::MessageSendingFlags,QString,Tp::TextChannelPtr)),
            this,
            SLOT(onSentMessage(Tp::Message,Tp::MessageSendingFlags,QString,Tp::TextChannelPtr)));

}

/*
 * Callback - when a message is received on a channel
 */
void SmsListener::onReceivedMessage(const Tp::ReceivedMessage &msg, const Tp::TextChannelPtr &chan) {
    Q_UNUSED(chan);

    qDebug() << "received msg:" << msg.text() << "from" << msg.sender()->id();

    QString token = "";
    if( msg.isDeliveryReport() ) {
        // 4 - DeliveryStatusAccepted
        // 1 - DeliveryStatusDelivered
        qDebug() << "delivery report:" << msg.deliveryDetails().status() <<
                    "from" << msg.deliveryDetails().originalToken();

        if( msg.deliveryDetails().status() == Tp::DeliveryStatusDelivered ) {
            emit SmsAcked(msg.deliveryDetails().originalToken());
        }

        return;
    }

    emit SmsRecv(token, msg.sender()->id(), msg.text());
}

void SmsListener::onSentMessage(const Tp::Message &msg, Tp::MessageSendingFlags flags,
                                const QString &token, const Tp::TextChannelPtr &chan) {
    qDebug() << "sent msg:" << msg.text() <<
                "to" << chan->targetContact()->id() << "(flags=" << int(flags) << ")";

    //NOTE: flags are not set
    /*
                (flags.testFlag(Tp::MessageSendingFlagReportDeleted)  ? "flag:deleted," : "") <<
                (flags.testFlag(Tp::MessageSendingFlagReportDelivery) ? "flag:delivery," : "") <<
                (flags.testFlag(Tp::MessageSendingFlagReportRead)     ? "flag:read," : "") <<
                ")";
    */

    QString contactName = getContact(chan->targetId());
    emit SmsSent(token, contactName, chan->targetId(), msg.text());
}


QString SmsListener::getContact(QString phoneNumber) {
    QContactDetailFilter df;
    df.setDetailType(QContactPhoneNumber::Type, QContactPhoneNumber::FieldNumber);
    df.setMatchFlags(QContactFilter::MatchPhoneNumber);
    df.setValue(phoneNumber);

    QList<QContact> contacts = contactMgr.contacts(df);
    // no matching contact founds
    if (contacts.length() == 0) {
        return QString();
    }

    QList<QContactDetail> details = contacts.first().details(QContactDetail::TypeDisplayLabel);
    try {
        return details.first().values().first().toString();
    } catch (std::exception &e) {}

    return QString();
}

