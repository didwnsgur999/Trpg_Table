#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QObject>
#include <QTcpSocket> // QTcpSocket을 사용하기 위해 포함
#include <QJsonDocument> // JSON 데이터 처리를 위해 포함
#include "chathandler.h" // ChatHandler 클래스 사용을 위해 포함

class ChatHandler; // 전방 선언

class ClientChat : public QObject
{
    Q_OBJECT // Qt의 시그널/슬롯 메커니즘을 사용하기 위해 필요

public:
    explicit ClientChat(QObject* parent = nullptr); // 생성자
    ~ClientChat(); // 소멸자

    // 서버에 소켓을 생성하고 연결을 시도하는 함수
    void makeSocket(const QString& ip, int port);

    // 서버로 JSON 데이터를 전송하는 함수
    void sendData(QJsonDocument& doc);

    // 서버 연결 상태를 반환하는 함수
    bool isConnected() const;

    // 소켓 에러 메시지를 반환하는 함수
    QString getSocketErrorString() const;

    // ChatHandler 인스턴스를 반환하는 getter 함수
    ChatHandler* getChatHandler() const;

signals:
    // 서버 연결 성공 시 방출되는 시그널
    void connectionEstablished();
    // 서버 연결 오류 시 방출되는 시그널
    void connectionError(QAbstractSocket::SocketError socketError);

private slots:
    // 소켓에서 데이터가 수신되었을 때 호출되는 슬롯
    void onReadyRead();
    // 소켓 상태가 변경되었을 때 호출되는 슬롯
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    // 소켓에 오류가 발생했을 때 호출되는 슬롯
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_clientSocket; // 클라이언트 소켓 포인터
    ChatHandler* m_chatHandler; // 채팅 데이터 처리를 위한 핸들러
    QByteArray m_buffer; // 부분적으로 수신된 데이터를 저장할 버퍼
};

#endif // CLIENTCHAT_H
