#include "clientchat.h"
#include <QtNetwork>

ClientChat::ClientChat(QObject* parent) {}

void ClientChat::makeSocket(QString& ip, int port){
    //이 아래에서 ui로부터 넘어온 포트, IP값 써야됨.
    m_clientSocket = new QTcpSocket(this);
    //data가 넘어오면 display해야함.
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&ClientChat::echoData);

    m_clientSocket->connectToHost(ip,port);
}

void ClientChat::echoData()
{
    QByteArray data = m_clientSocket->readAll();
    QString message = QString::fromUtf8(data);

    emit newMessage(message);
}

void ClientChat::sendData(QString& mesg){
    if(mesg.length()){
        QByteArray bytearray;
        bytearray = mesg.toUtf8();
        m_clientSocket->write(bytearray);
    }
}
