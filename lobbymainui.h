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
#include "storeui.h"
#include "roomdisplayui.h"

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
    //change left page
    void changeLeftPage(int index);

signals:
    void requestPageChange(int index);

private slots:

    //void on_createChatRoomButton_clicked();
    //void on_enterChatRoomButton_clicked(); // 입장/나가기 겸용 버튼 슬롯
    void on_exitChatRoomButton_clicked(); // 실제 UI 버튼은 없지만, 로직 분리를 위해 유지

    void requestRoomList();
    //void updateRoomList(const QJsonArray& roomList);
    void handleRoomCreationResult(bool success, const QString& message);
    void handleRoomJoinResult(bool success, const QString& message, const QString& roomName);
    //void handleRoomLeaveResult(bool success, const QString& message);

    void sendJoinRoomRequest(const QString& roomName);

    void on_leftTabWidget_currentChanged(int index);

private:
    Ui::LobbyMainUI* ui;
    ClientChat* m_clientChat;
    RoomListUI* m_roomListUI;
    ChatRoomUI* m_chatRoomUI;
    ChatHandler* m_chatHandler;

    //store ui
    storeUI* m_storeUI;
    RoomDisplayUI* m_roomDisplayUI;
};

#endif // LOBBYMAINUI_H
