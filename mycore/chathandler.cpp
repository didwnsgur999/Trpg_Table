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
    //qDebug() << "처리 중인 CMD: " << cmd; // 처리 중인 CMD 출력

    if(cmd == "ret_chat"){ //채팅 수신
        QString chatText = obj.value("text").toString();
        //qDebug() << "채팅 메시지 수신: " << chatText;
        emit chatreceived(chatText);
    } else if (cmd == "ret_login") { // 서버로부터 로그인 결과가 왔을 때
        bool success = (obj.value("text")=="success");
        QString message = obj.value("message").toString();
        QJsonObject cus = obj.value("cus").toObject();
        //qDebug() << "로그인 결과 수신: 성공=" << success << ", 메시지=" << message;
        emit loginResult(success, message,cus); // LoginUI로 결과 전달
    } else if (cmd=="ret_add_c"){
        bool success = (obj.value("text")=="success");
        QString message = obj.value("message").toString();
        QJsonObject cus = obj.value("cus").toObject();
        //qDebug() << "회원가입 결과 수신: 성공=" << success << ", 메시지=" << message;
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
    } else if (cmd == "ret_leave_r") { // 방 나가기 결과 응답 처리
        bool success = (obj.value("text").toString() == "success");
        QString message = obj.value("text").toString();
        emit leaveRoomResult(success, message);
    } else if (cmd=="ret_list_p"){ // 제품 정보 응답 처리
        QJsonArray products = obj.value("productlist").toArray();
        emit productListReceived(products);
    } else if (cmd=="ret_add_o"){ // 주문 정보 추가 처리
        //추가 예정
        bool success = (obj.value("text").toString() == "success");
    } else if (cmd=="ret_add_r_item"){ //채팅방 아이템 추가 처리
        bool success = (obj.value("text").toString()=="success");
        if(success) emit addRoomItemResult(obj);
    } else if (cmd=="ret_del_r_item"){ //채팅방 아이템 제거 처리
        bool success = (obj.value("text").toString()=="success");
        if(success) emit delRoomItemResult(obj);
    } else if (cmd=="ret_mov_r_item"){ //채팅방 아이템 위치 처리
        bool success = (obj.value("text").toString()=="success");
        if(success) emit movRoomItemResult(obj);
    } else if (cmd=="ret_list_users"){ // 전체인원 응답 처리
        QJsonArray userList = obj["users"].toArray();
        emit AllUserListReceived(userList);
    } else if (cmd=="ret_list_r_users"){ //채팅방 입장인원 응답 처리
        QJsonArray roomUserList = obj["rusers"].toArray();
        emit roomUserListReceived(roomUserList);
    } else if (cmd=="ret_list_r_items"){ //채팅방 아이템 전체 응답 처리
        QJsonArray roomItemList = obj["rItems"].toArray();
        emit roomItemListReceived(roomItemList);
    } else if (cmd=="ret_invite_r"){ // 초대 응답 처리
        bool success = (obj.value("text").toString()=="success");
        if(success){ //
            QString rName = obj["rName"].toString();
            emit inviteReceived(rName);
        } else { //내가 초대하고 초대 무효됬을때.
            emit inviteFailed();
        }
    } else if (cmd=="ret_ban_r"){
        QString message = obj.value("text").toString();
        emit banreceived(message);
    } else if (cmd=="ret_banned_r"){ //
        emit bannedreceived();
    }
}
