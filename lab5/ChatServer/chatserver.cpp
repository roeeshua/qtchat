#include "chatserver.h"
#include "serverworker.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent)
{

}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    Serverworker *worker = new Serverworker(this);
    if(!worker->setSocketDescriptor(socketDescriptor)){
        worker->deleteLater();
        return;
    };
    connect(worker,&Serverworker::logMessage,this,&ChatServer::logMessage);
    connect(worker,&Serverworker::jsonReceived, this, &ChatServer::jsonReceived);
    connect(worker,&Serverworker::disconnectedFromClient, this,
        std::bind(&ChatServer::userDisconnected,this,worker));
    m_clients.append(worker);
    emit logMessage("新的用户连接上了");
}

void ChatServer::broadcast(const QJsonObject &message, Serverworker *exclude)
{
    for(Serverworker *worker: m_clients){
        worker->sendJson(message);
    }
}

void ChatServer::stopServer()
{
    close();
}

void ChatServer::jsonReceived(Serverworker *sender, const QJsonObject &docObj)
{
    const QJsonValue typeVal=docObj.value("type");
    if(typeVal.isNull()||!typeVal.isString())
        return;
    if(typeVal.toString().compare("message",Qt::CaseInsensitive)==0){
        const QJsonValue textVal= docObj.value("text");
        if(textVal.isNull()||!textVal.isString())
            return;
        const QString text =textVal.toString().trimmed();
        if(text.isEmpty())
            return;
        QJsonObject message;
        message["type"]="message";
        message["text"]= text;
        message["sender"]=sender->userName();
        broadcast(message,sender);
    }else if(typeVal.toString().compare("login",Qt::CaseInsensitive)==0){
        const QJsonValue usernameVal= docObj.value("text");
        if(usernameVal.isNull()||!usernameVal.isString())
            return;
        sender->setUserName(usernameVal.toString());
        QJsonObject connectedMessage;
        connectedMessage["type"]="newuser";
        connectedMessage["username"]=usernameVal.toString();
        broadcast(connectedMessage,sender);

        //send user list to new logined user
        QJsonObject userlistMessage;
        userlistMessage["type"]="userlist";
        QJsonArray userlist;
        for(Serverworker *worker :m_clients){
            if(worker == sender)
                userlist.append(worker->userName()+"*");
            else
                userlist.append(worker->userName());
        }
        userlistMessage["userlist"]= userlist;
        sender->sendJson(userlistMessage);
    }
}

void ChatServer::userDisconnected(Serverworker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if(!userName.isEmpty()){
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"]="userdisconnected";
        disconnectedMessage["username"]= userName;
        broadcast(disconnectedMessage, nullptr);
        emit logMessage(userName + " disconnected");
    }
    sender->deleteLater();
}

