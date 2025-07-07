#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QObject>
#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QAbstractSocket>
#include "chathandler.h"
// #include "lobbyui.h" LobbyUI 의존성 제거

class ClientChat : public QObject
{
    Q_OBJECT
public:
    explicit ClientChat(QObject *parent = nullptr);

    //socket 만들고 연결하기. m_lobbyUI를 넣는게 불만이긴하다. -> lobbyUI 포인터 제거
    void makeSocket(const QString& ip, int port);
    void echoData(); // QTcpSocket::readyRead시그널에 연결될 슬롯
    void sendData(QJsonDocument &doc);

    ChatHandler* getChatHandler(){return m_chatHandler;}
    bool isConnected() const; // 서버 연결 상태 확인 메서드 추가
    QString getSocketErrorString() const; // 소켓 에러 메시지 반환 메서드 추가

signals:
    void connectionEstablished(); // 서버 연결 성공 시그널
    void connectionError(QAbstractSocket::SocketError socketError); // 서버 연결 오류 시그널

private slots:
    void onSocketStateChanged(QAbstractSocket::SocketState socketState); // 소켓 상태 변경 슬롯
    void onSocketError(QAbstractSocket::SocketError socketError); // 소켓 에러 슬롯

private:
    QTcpSocket* m_clientSocket;
    ChatHandler* m_chatHandler;
};

#endif // CLIENTCHAT_H
