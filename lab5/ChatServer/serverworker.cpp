#include "serverworker.h"
#include<QDataStream>
#include<QJsonObject>
#include<QJsonDocument>
Serverworker::Serverworker(QObject *parent)
    : QObject{parent}
{
    m_serverSocket = new QTcpSocket(this);
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&Serverworker::onReadyRead);
}

bool Serverworker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void Serverworker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_6_7);// start an infinite loop
    for(;;){
        socketStream.startTransaction();
        socketStream>>jsonData;
        if(socketStream.commitTransaction()){
            emit logMessage(QString::fromUtf8(jsonData));
            sendMessage("I recieved message");
        }else{
            break;
        }
    }
}

void Serverworker::sendMessage(const QString &text,const QString &type)
{
    if(m_serverSocket->state()!= QAbstractSocket::ConnectedState)
        return;
    if(!text.isEmpty()){
        // create a QDatastream operating on the socket
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Qt_6_7);
        // Create the JSoN we want to send
        QJsonObject message;
        message["type"]=type;
        message["text"]= text;
        //send the JSON using ODatastream
        serverStream<<QJsonDocument(message).toJson();
    }
}
