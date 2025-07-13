#ifndef LOBBYMAINUI_H
#define LOBBYMAINUI_H

#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QJsonArray>

#include "clientchat.h"
#include "roomlistui.h"
#include "chatroomui.h" // 새로 생성된 ChatRoomUI 포함

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
    void changePage(int index);

signals:
    void requestPageChange(int index);

private slots:
    void on_goToShopButton_clicked();
    void on_goToMapButton_clicked();

    //void on_createChatRoomButton_clicked();
    //void on_enterChatRoomButton_clicked(); // 입장/나가기 겸용 버튼 슬롯
    void on_exitChatRoomButton_clicked(); // 실제 UI 버튼은 없지만, 로직 분리를 위해 유지

    void requestRoomList();
    //void updateRoomList(const QJsonArray& roomList);
    void handleRoomCreationResult(bool success, const QString& message);
    void handleRoomJoinResult(bool success, const QString& message, const QString& roomName);
    //void handleRoomLeaveResult(bool success, const QString& message);

    void sendJoinRoomRequest(const QString& roomName);

private:
    ClientChat* m_clientChat;
    Ui::LobbyMainUI* ui; // .ui 파일을 통해 만들어진 자동 생성 UI멤버들을 나의 클래스에서 접근하기 위한 다리 역할
    RoomListUI* m_roomListUI;
    ChatRoomUI* m_chatRoomUI;
    ChatHandler* m_chatHandler;
};

#endif // LOBBYMAINUI_H
