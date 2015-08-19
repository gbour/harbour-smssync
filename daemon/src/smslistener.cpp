
#include <QDebug>

#include <TelepathyQt/Debug>
#include <TelepathyQt/Constants>
#include <TelepathyQt/Types>

#include <TelepathyQt/TextChannel>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/ReceivedMessage>

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

        if( msg.deliveryDetails().status() == Tp::DeliveryStatusAccepted ) {
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

    emit SmsSent(token, chan->targetContact()->id(), msg.text());
}



