#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include "serverworker.h"

class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
    QVector<Serverworker *>m_clients;

    void broadcast(const QJsonObject &message,Serverworker *exclude);

signals:
    void logMessage(const QString& msg);

public slots:
    void stopServer();
    void jsonReceived(Serverworker *sender,const QJsonObject &docObj);
    void userDisconnected(Serverworker *sender);
};

#endif // CHATSERVER_H
