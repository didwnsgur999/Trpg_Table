#include "clientchat.h"
#include <QtNetwork> // QTcpSocket, QHostAddress 등 네트워크 관련 클래스 포함
#include <QDebug> // qDebug() 사용을 위해 포함

ClientChat::ClientChat(QObject* parent)
    : QObject(parent) // QObject를 부모로 설정
    , m_clientSocket(nullptr) // m_clientSocket 초기화 nullptr
{
    m_chatHandler = new ChatHandler(this); // ChatHandler 인스턴스 생성
}

ClientChat::~ClientChat()
{
    if (m_clientSocket) {
        m_clientSocket->abort(); // 소켓 강제 종료 (연결되어 있다면)
        m_clientSocket->deleteLater(); // 안전하게 객체 삭제 예약
    }
    // m_chatHandler는 부모가 ClientChat이므로 ClientChat 소멸 시 함께 소멸됨
}

// 소켓 생성하고 서버에 연결
void ClientChat::makeSocket(const QString& ip, int port){
    qDebug() << "ClientChat::makeSocket 호출. IP:" << ip << ", Port:" << port;

    // 기존 소켓이 연결되어 있거나 연결 시도 중이면 정리
    if(m_clientSocket){
        if(m_clientSocket->state() != QAbstractSocket::UnconnectedState){
            m_clientSocket->abort(); // 강제 연결 종료
            qDebug() << "기존 소켓 연결 종료 시도.";
        }
        m_clientSocket->deleteLater();  // 안전하게 객체 삭제 예약
        m_clientSocket = nullptr;
        qDebug() << "기존 소켓 정리 완료.";
    }

    m_clientSocket = new QTcpSocket(this); // 새로운 소켓 인스턴스 생성

    // 소켓 시그널과 슬롯 연결
    connect(m_clientSocket, &QAbstractSocket::readyRead, this, &ClientChat::onReadyRead); // 데이터 수신 시그널
    connect(m_clientSocket, &QAbstractSocket::stateChanged, this, &ClientChat::onSocketStateChanged); // 소켓 상태 변화 감지
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred), this, &ClientChat::onSocketError); // 에러 발생 시그널

    // 서버에 연결 시도
    m_clientSocket->connectToHost(ip, port);
    qDebug() << "서버 연결 시도 중...";
}

// 소켓에서 데이터가 수신되었을 때 호출되는 슬롯
void ClientChat::onReadyRead(){
    QByteArray data = m_clientSocket->readAll(); // 모든 수신 데이터 읽기
    m_buffer.append(data); // 버퍼에 추가

    // 줄바꿈 문자를 기준으로 메시지 분리 및 처리
    while (m_buffer.contains('\n')) {
        int newlineIndex = m_buffer.indexOf('\n');
        QByteArray message = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1); // 처리한 메시지 및 줄바꿈 제거

        m_chatHandler->getByteData(message); // ChatHandler로 데이터 전달
    }
}

// 서버로 JSON 데이터를 전송하는 함수
void ClientChat::sendData(QJsonDocument& doc){
    //socket 연결이 되어있고, state가 정상상태일때 전송
    if(m_clientSocket && m_clientSocket->state() == QAbstractSocket::ConnectedState){
        if(!doc.isEmpty()){
            // JSON 메시지 끝에 줄바꿈 추가 (메시지 단위 구분용)
            QByteArray bytearray = doc.toJson(QJsonDocument::Compact) + "\n";
            m_clientSocket->write(bytearray); // 서버로 데이터 전송
            m_clientSocket->flush(); // 버퍼 비우기
        }
    }else{// NetworkError시 예외처리
        emit connectionError(QAbstractSocket::NetworkError);
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

// ChatHandler 인스턴스를 반환하는 getter 함수
ChatHandler* ClientChat::getChatHandler() const
{
    return m_chatHandler;
}

// 소켓 상태 변경 슬롯
void ClientChat::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "소켓 상태 변경" << socketState;
    if(socketState == QAbstractSocket::ConnectedState){
        qDebug() << "서버 연결 성공";
        emit connectionEstablished(); // 연결 성공 시 시그널 방출
    } else if (socketState == QAbstractSocket::UnconnectedState) {
        // 연결 끊김 또는 연결 실패 시
        qDebug() << "서버 연결 끊김 또는 실패.";
        // connectionError 시그널은 onSocketError에서 주로 처리하므로 여기서는 방출하지 않음
    }
}

// 소켓에 오류가 발생했을 때 호출되는 슬롯
void ClientChat::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "소켓 오류 발생: " << m_clientSocket->errorString();
    emit connectionError(socketError); // 오류 시그널 방출
}
