#include "chathandler.h"
#include <QJsonParseError>

ChatHandler::ChatHandler(QObject* parent)
    : QObject(parent)
{}

void ChatHandler::getByteData(QByteArray& data){
    qDebug() << "Received raw data:" << data; // 수신된 전체 데이터 출력

    // QByteArray가 여러 JSON 객체를 포함할 수 있으므로, 줄바꿈으로 분리하여 처리
    QList<QByteArray> messages = data.split('\n');
    for (const QByteArray& msg : messages) {
        if (msg.isEmpty()) continue; // 빈 메시지 스킵

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(msg, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "JSON 파싱 오류: " << err.errorString();
            qDebug() << "잘못된 데이터: " << msg;
            continue; // 다음 메시지로
        }

        QJsonObject obj = doc.object();
        processJsonObject(obj); // 새로운 헬퍼 함수 호출
    }
}

// ret_{cmd} 형태로의 반환 반영!!!
void ChatHandler::processJsonObject(const QJsonObject &obj)
{
    QString cmd = obj.value("cmd").toString();
    qDebug() << "처리 중인 CMD: " << cmd; // 처리 중인 CMD 출력

    if(cmd == "chat"){
        QString chatText = obj.value("text").toString();
        qDebug() << "채팅 메시지 수신: " << chatText;
        emit chatreceived(chatText);
    } else if (cmd == "ret_login") { // 서버로부터 로그인 결과가 왔을 때
        bool success = (obj.value("text")=="success");
        QString message = obj.value("message").toString();
        qDebug() << "로그인 결과 수신: 성공=" << success << ", 메시지=" << message;
        emit loginResult(success, message); // LoginUI로 결과 전달
    // 0707 추가기능
    }else if (cmd == "ret_list_rooms") { // 채팅방 목록 응답 처리
        QJsonArray rooms = obj.value("rooms").toArray();
        emit roomListReceived(rooms);
    } else if (cmd == "ret_add_r") { // 방 생성 결과 응답 처리
        bool success = (obj.value("text").toString() == "success");
        QString message = obj.value("text").toString();
        emit addRoomResult(success, message);
    } else if (cmd == "ret_join_r") { // 방 입장 결과 응답 처리
        bool success = (obj.value("text").toString() == "success");
        QString message = obj.value("text").toString();
        emit joinRoomResult(success, message);
        // 성공 시 LobbyMainUI에서 실제 채팅방 UI로 전환 로직 추가 필요
    } else if (cmd == "ret_leave_r") { // 방 나가기 결과 응답 처리 (추가된 프로토콜)
        bool success = (obj.value("text").toString() == "success");
        QString message = obj.value("text").toString();
        emit leaveRoomResult(success, message);
    }
}
