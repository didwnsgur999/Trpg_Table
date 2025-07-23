#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray> // QJsonArray 사용을 위함
#include <QDebug> // 디버깅용

class ChatHandler : public QObject
{
    Q_OBJECT
public:
    explicit ChatHandler(QObject* parent = nullptr);

    void getByteData(QByteArray& data);

signals:
    void chatreceived(const QString& msg);
    // 새로 추가된 시그널: 로그인 결과 (성공 여부, 메시지)
    void loginResult(bool success, const QString& message, const QJsonObject& cus);
    void registerResult(bool success, const QString& message,const QJsonObject& cus);

    // 0707 추가된 시그널
    void roomListReceived(const QJsonArray& roomList); // 채팅방 목록 수신 시그널
    void addRoomResult(bool success, const QString& message); // 방생성 결과
    void joinRoomResult(bool success, const QString& roomName); // 방입장 결과
    void leaveRoomResult(bool success, const QString& message); // 방나가기 결과

    // 0713 추가된 시그널
    void productListReceived(const QJsonArray& products);

    // 0716 roomitem 시그널
    void addRoomItemResult(const QJsonObject& product);
    void delRoomItemResult(const QJsonObject& product);
    void movRoomItemResult(const QJsonObject& product);
    // 0721 room
    void roomUserListReceived(const QJsonArray& RoomUserList);
    void AllUserListReceived(const QJsonArray& UserList);
    void roomItemListReceived(const QJsonArray& RoomItemList);

private:
    // Json 파싱 후 처리하는 헬퍼 함수
    void processJsonObject(const QJsonObject& obj);
};

#endif // CHATHANDLER_H
