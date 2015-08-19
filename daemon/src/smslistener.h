#ifndef SMSLISTENER_H
#define SMSLISTENER_H

#include <QObject>

#include <TelepathyQt/Types>
#include <TelepathyQt/Account>
#include <TelepathyQt/SimpleTextObserver>
#include <TelepathyQt/Message>

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
    void SmsRecv(QString id, QString from, QString content);
    void SmsSent(QString id, QString to, QString content);
    void SmsAcked(QString id);

public slots:

private slots:
    void onAccountReady(Tp::PendingOperation *op);
    void onReceivedMessage(const Tp::ReceivedMessage&, const Tp::TextChannelPtr&);
    void onSentMessage(const Tp::Message &msg, Tp::MessageSendingFlags flags,
                       const QString &token, const Tp::TextChannelPtr &chan);

private:
    Tp::AccountPtr account;
    Tp::SimpleTextObserverPtr observer;
};

#endif // SMSLISTENER_H
