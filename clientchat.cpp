#include "clientchat.h"
#include <QtNetwork>

ClientChat::ClientChat(QObject* parent) {
    m_chatHandler = new ChatHandler(this);
}
void ClientChat::makeSocket(QString& ip, int port,lobbyUI* m_lobbyUI){
    qDebug()<<"makeSocket";
    //일단 이게 문제긴하네.
    // if (m_clientSocket) {
    //     m_clientSocket->disconnectFromHost();
    //     m_clientSocket->deleteLater();  // 안전하게 지움
    //     m_clientSocket = nullptr;
    // }

    //이 아래에서 ui로부터 넘어온 포트, IP값 써야됨.
    m_clientSocket = new QTcpSocket(this);

    //error발생시 lobbyui에 display
    connect(m_clientSocket,&QAbstractSocket::errorOccurred,m_lobbyUI,&lobbyUI::on_join_server_error);
    m_clientSocket->connectToHost(ip,port);
    //data가 넘어오면 display해야함.
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&ClientChat::echoData);
}

void ClientChat::echoData()
{
    QByteArray data = m_clientSocket->readAll();
    m_chatHandler->getByteData(data);
}

void ClientChat::sendData(QJsonDocument& doc){
    if(!doc.isEmpty()){
        QByteArray bytearray;
        bytearray = doc.toJson(QJsonDocument::Compact);
        bytearray.append('\n');             //구분자
        m_clientSocket->write(bytearray);
    }
}
