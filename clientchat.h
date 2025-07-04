#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include "lobbyui.h"
#include "chathandler.h"

class ClientChat : public QObject
{
    Q_OBJECT
public:
    ClientChat(QObject *parent=nullptr);

    //socket 만들고 연결하기. m_lobbyUI를 넣는게 불만이긴하다.
    void makeSocket(QString& ip, int port,lobbyUI* m_lobbyUI);

    void echoData();
    void sendData(QJsonDocument& doc);

    //getter/setter
    void setClientSocket(QTcpSocket* _clientSocket) { m_clientSocket=_clientSocket; }
    ChatHandler* getChatHandler(){return m_chatHandler;}

signals:
    void newMessage(QString& message);

private:
    QTcpSocket* m_clientSocket;
    ChatHandler* m_chatHandler;
};

#endif // CLIENTCHAT_H
