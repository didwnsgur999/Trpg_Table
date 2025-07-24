#ifndef LOBBYMAINUI_H
#define LOBBYMAINUI_H

#include <QWidget>
#include <QStackedWidget>

#include "mycore/clientchat.h"
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
    void changePage(const QString& roomName);

signals:
    void requestPageChange(int index);
    void requestLeaveRoom();
    void enterChatRoom();
    void acceptInvite(const QString& rName);

private slots:

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
    //첫 생성시 무시
    bool m_firstTabChangeIgnored = false;
    bool m_firstStackChangeIgnored = false;
};

#endif // LOBBYMAINUI_H
