#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <memory.h>

class ClientChat : public QObject
{
    Q_OBJECT
public:
    ClientChat(QObject *parent=nullptr);
    void setClientSocket(QTcpSocket* _clientSocket) { m_clientSocket=_clientSocket; }
    void makeSocket(QString& ip, int port);

    void echoData();
    void sendData(QString& mesg);

signals:
    void newMessage(QString& message);

private:
    QTcpSocket* m_clientSocket;
};

#endif // CLIENTCHAT_H
