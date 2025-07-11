#include "chathandler.h"

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
        QString cmd = obj.value("cmd").toString();

        qDebug() << "처리 중인 CMD: " << cmd; // 처리 중인 CMD 출력

        if(cmd == "chat"){
            QString chatText = obj.value("text").toString();
            qDebug() << "채팅 메시지 수신: " << chatText;
            emit chatreceived(chatText);
        } else if (cmd == "login_result") { // 서버로부터 로그인 결과가 왔을 때
            bool success = obj.value("success").toBool();
            QString message = obj.value("message").toString();
            qDebug() << "로그인 결과 수신: 성공=" << success << ", 메시지=" << message;
            emit loginResult(success, message); // LoginUI로 결과 전달
        }
        // 향후 다른 CMD (예: buy_map_result, chat_room_list 등) 처리 로직 추가
    }
}
