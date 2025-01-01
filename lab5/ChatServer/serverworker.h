#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
class Serverworker : public QObject
{
    Q_OBJECT
public:
    explicit Serverworker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);

    QString userName();
    void setUserName(QString user);

signals:
    void logMessage(const QString& msg);
    void jsonReceived(Serverworker*sender,const QJsonObject &docObj);
private:
    QTcpSocket *m_serverSocket;
    QString m_userName;

public slots:
    void onReadyRead();
    void sendMessage(const QString &text, const QString &type = "message");
    void sendJson(const QJsonObject &json);
};

#endif // SERVERWORKER_H
