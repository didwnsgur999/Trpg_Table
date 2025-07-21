#include "chathandler.h"
#include <QJsonParseError>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

ChatHandler::ChatHandler(QObject* parent)
    : QObject(parent)
{}

void ChatHandler::getByteData(QByteArray& data){
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

    if(cmd == "ret_chat"){
        QString chatText = obj.value("text").toString();
        qDebug() << "채팅 메시지 수신: " << chatText;
        emit chatreceived(chatText);
    } else if (cmd == "ret_login") { // 서버로부터 로그인 결과가 왔을 때
        bool success = (obj.value("text")=="success");
        QString message = obj.value("message").toString();
        QJsonObject cus = obj.value("cus").toObject();
        qDebug() << "로그인 결과 수신: 성공=" << success << ", 메시지=" << message;
        emit loginResult(success, message,cus); // LoginUI로 결과 전달
    } else if (cmd=="ret_add_c"){
        bool success = (obj.value("text")=="success");
        QString message = obj.value("message").toString();
        QJsonObject cus = obj.value("cus").toObject();
        qDebug() << "회원가입 결과 수신: 성공=" << success << ", 메시지=" << message;
        emit registerResult(success, message,cus); // LoginUI로 결과 전달
    } else if (cmd == "ret_list_r") { // 채팅방 목록 응답 처리
        QJsonArray rooms = obj.value("roomlist").toArray();
        emit roomListReceived(rooms);
    } else if (cmd == "ret_add_r") { // 방 생성 결과 응답 처리
        bool success = (obj.value("text").toString() == "success");
        QString message = obj.value("text").toString();
        emit addRoomResult(success, message);
    } else if (cmd == "ret_join_r") { // 방 입장 결과 응답 처리
        bool success = (obj.value("text").toString() == "success");
        QString roomName = obj.value("rName").toString();
        emit joinRoomResult(success, roomName);
        // 성공 시 LobbyMainUI에서 실제 채팅방 UI로 전환 로직 추가 필요
    } else if (cmd == "ret_leave_r") { // 방 나가기 결과 응답 처리
        bool success = (obj.value("text").toString() == "success");
        QString message = obj.value("text").toString();
        emit leaveRoomResult(success, message);
    } else if (cmd=="ret_list_p"){
        QJsonArray products = obj.value("productlist").toArray();
        emit productListReceived(products);
    } else if (cmd=="ret_add_o"){
        bool success = (obj.value("text").toString() == "success");
        qDebug()<<"order추가 성공 : "<<success;
    } else if (cmd=="ret_add_r_item"){
        //방에 아이템 추가됬음 -> 그림은 Backend에 있다고 생각하자.
        bool success = (obj.value("text").toString()=="success");
        if(success) emit addRoomItemResult(obj);
    } else if (cmd=="ret_del_r_item"){
        bool success = (obj.value("text").toString()=="success");
        if(success) emit delRoomItemResult(obj);
    } else if (cmd=="ret_mov_r_item"){
        bool success = (obj.value("text").toString()=="success");
        if(success) emit movRoomItemResult(obj);
    } else if (cmd=="ret_list_users"){
        QJsonArray userList = obj["users"].toArray();
        emit AllUserListReceived(userList);
    } else if (cmd=="ret_list_r_users"){
        QJsonArray roomUserList = obj["rusers"].toArray();
        emit roomUserListReceived(roomUserList);
    }
}
