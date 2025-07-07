#include "clientchat.h"
#include <QtNetwork>
#include <QDebug>

ClientChat::ClientChat(QObject* parent)
    : QObject(parent) // QObject를 부모로 설정
    , m_clientSocket(nullptr) // m_clientSocket 초기화 nullptr
{
    m_chatHandler = new ChatHandler(this);
}

// 소켓 생성하고 서버에 연결 + lobbyUI 매개변수 제거
void ClientChat::makeSocket(const QString& ip, int port){ // 시그니처 변경 반영
    qDebug()<<"makeSocket 호출";

    // 기존 소켓이 연결되어 있거나 연결 시도 중이면 정리
    if(m_clientSocket){
        if(m_clientSocket->state() != QAbstractSocket::UnconnectedState){
            m_clientSocket->abort(); // 강제 연결 종료
        }
        m_clientSocket->deleteLater();  // 안전하게 객체 삭제 예약
        m_clientSocket = nullptr;
    }

    // 이 아래에서 ui로부터 넘어온 포트, IP값 써야됨.
    m_clientSocket = new QTcpSocket(this); // 새로운 소켓 인스턴스 생성

    // 소켓 상태 변화 감지 (연결 성공 시그널을 위함)
    connect(m_clientSocket,&QAbstractSocket::stateChanged, this, &ClientChat::onSocketStateChanged); // 소켓 상태 변화 감지
    // 에러 발생 시그널 연결 (LoginUI로 전달)
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred), this, &ClientChat::onSocketError);
    // 데이터 수신 시그널 연결
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ClientChat::echoData);

    // 서버 연결 시도 (비동기)
    m_clientSocket->connectToHost(ip,port);
    qDebug() << "서버 연결 시도 IP " << ip << ", Port: " << port;
}

void ClientChat::echoData()
{
    QByteArray data = m_clientSocket->readAll(); // 모든 수신 데이터 읽기
    m_chatHandler->getByteData(data); // ChatHandler로 데이터 전달
}

void ClientChat::sendData(QJsonDocument& doc){
    if(m_clientSocket && m_clientSocket->state() == QAbstractSocket::ConnectedState){
        if(!doc.isEmpty()){
            QByteArray bytearray = doc.toJson(QJsonDocument::Compact) + "\n"; // Json에 줄바꿈 추가
            m_clientSocket->write(bytearray); // 서버로 데이터 전송
            m_clientSocket->flush(); // 버퍼 비우기
            qDebug() << "데이터 전송 완료 " << bytearray;
        }
    }else{
            qDebug() << "오류: 서버 미연결로 데이터 전송 불가";
    }
}

// 서버 연결 상태 반환
bool ClientChat::isConnected() const
{
    return m_clientSocket && m_clientSocket->state() == QAbstractSocket::ConnectedState;
}

// 소켓 에러 메시지 반환
QString ClientChat::getSocketErrorString() const
{
    if(m_clientSocket){
        return m_clientSocket->errorString();
    }
    return "소켓 없음";
}

// 소켓 상태 변경 슬롯
void ClientChat::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "소켓 상태 변경" << socketState;
    if(socketState == QAbstractSocket::ConnectedState){
        qDebug() << "서버 연결 성공";
        emit connectionEstablished(); // 연결 성공 시그널
    } else if(socketState == QAbstractSocket::UnconnectedState){
        // 연결 해제 또는 초기 연결 실패시의 상태
        qDebug() << "서버 연결 해제 또는 실패";
    }
}

// 소켓 에러 슬롯
void ClientChat::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "소켓 에러" << m_clientSocket->errorString();
    emit connectionError(socketError); // 로그인 UI로 에러 시그널 전달
}
