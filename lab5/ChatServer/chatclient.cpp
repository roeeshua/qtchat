#include "../ChatClient/chatclient.h"
#include<QDataStream>
#include<QJsonObject>
#include<QJsonDocument>

ChatClient::ChatClient(QObject *parent)
    : QObject{parent}
{
    m_clientSocket =new QTcpSocket(this);

    connect(m_clientSocket,&QTcpSocket::connected,this,&ChatClient::connected);
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&ChatClient::onReadyRead);
}

void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_6_7);// start an infinite loop
    for(;;){
        socketStream.startTransaction();
        socketStream>>jsonData;
        if(socketStream.commitTransaction()){
            emit messageReceived(QString::fromUtf8(jsonData));
            sendMessage("I recieved message");
        }else{
            break;
        }
    }
}

void ChatClient::sendMessage(const QString &text, const QString &type)
{
    if(m_clientSocket->state()!= QAbstractSocket::ConnectedState)
        return;
    if(!text.isEmpty()){
        // create a QDatastream operating on the socket
        QDataStream serverStream(m_clientSocket);
        serverStream.setVersion(QDataStream::Qt_6_7);
        // Create the JSoN we want to send
        QJsonObject message;
        message["type"]=type;
        message["text"]= text;
        //send the JSON using ODatastream
        serverStream<<QJsonDocument(message).toJson();
    }
}

void ChatClient::connectToserver(const QHostAddress &address, quint16 port)
{

}