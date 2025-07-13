#ifndef LOBBYMAINUI_H
#define LOBBYMAINUI_H

#include <QWidget>
#include <QStackedWidget>

#include "clientchat.h"
#include "roomlistui.h"
#include "chatroomui.h"

namespace Ui{ // Ui파일과 연결하기 위한 자동생성 클래스(알아서 추가됨)
class LobbyMainUI;
}
class LobbyMainUI : public QWidget
{
    Q_OBJECT

public:
    explicit LobbyMainUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~LobbyMainUI();

    void initializeLobby();

public slots:
    void changePage(const QString& roomName);

signals:
    void requestPageChange(int index);

private slots:
    //void on_createChatRoomButton_clicked();
    //void on_enterChatRoomButton_clicked(); // 입장/나가기 겸용 버튼 슬롯
    //void on_exitChatRoomButton_clicked(); // 실제 UI 버튼은 없지만, 로직 분리를 위해 유지

    //void handleJoinRoomRequest(const QString& roomName); // 방생성

    //void requestRoomList();
    //void updateRoomList(const QJsonArray& roomList);
    //void handleRoomCreationResult(bool success, const QString& message); -> 옮김(룸)
    //void handleRoomJoinResult(bool success, const QString& message, const QString& roomName);
    //void handleRoomLeaveResult(bool success, const QString& message);

    //void on_exitChatRoomButton_clicked(); // 나가기 슬롯은 계속 사용
    //void handleCreateAndJoinRoom(const QString& roomName); // 통합 슬롯
    //void handleRoomCreationResult(bool success, const QString& message); // 생성 결과 응답 처리 -> 옮김
    //void handleRoomJoinResult(bool success, const QString& message, const QString& roomName); // 입장 결과 처리 -> 옮김

    //void sendCreateRoomRequest(const QString& roomName);
    //void sendJoinRoomRequest(const QString& roomName);
    //void handleRoomJoinResult(bool success, const QString& message, const QString& roomName);


private:
    ClientChat* m_clientChat;
    Ui::LobbyMainUI* ui; // .ui 파일을 통해 만들어진 자동 생성 UI멤버들을 나의 클래스에서 접근하기 위한 다리 역할
    RoomListUI* m_roomListUI;
    ChatRoomUI* m_chatRoomUI;
    //ChatHandler* m_chatHandler;
};

#endif // LOBBYMAINUI_H
