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
#include "chatroomui.h" // 새로 생성된 ChatRoomUI 포함

class LobbyMainUI : public QWidget
{
    Q_OBJECT

public:
    explicit LobbyMainUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~LobbyMainUI();

    void initializeLobby();

signals:
    void requestPageChange(int index);

private slots:
    void on_goToShopButton_clicked();
    void on_goToMapButton_clicked();

    void on_createChatRoomButton_clicked();
    void on_enterChatRoomButton_clicked(); // 입장/나가기 겸용 버튼 슬롯
    void on_exitChatRoomButton_clicked(); // 실제 UI 버튼은 없지만, 로직 분리를 위해 유지

    void requestRoomList();
    void updateRoomList(const QJsonArray& roomList);
    void handleRoomCreationResult(bool success, const QString& message);
    void handleRoomJoinResult(bool success, const QString& message, const QString& roomName);
    void handleRoomLeaveResult(bool success, const QString& message);

    void sendJoinRoomRequest(const QString& roomName);

private:
    ClientChat* m_clientChat;

    QSplitter* m_mainSplitter;

    QWidget* m_leftPaneWidget;
    QStackedWidget* m_leftStackedWidget;
    QWidget* m_mapDisplayWidget;
    QWidget* m_shopWidget;
    QPushButton* m_leftShopButton;
    QPushButton* m_leftMapButton;

    QWidget* m_rightPaneWidget;
    QLabel* m_welcomeLabel;
    QStackedWidget* m_rightStackedWidget;
    QWidget* m_chatRoomListWidget;
    ChatRoomUI* m_currentChatRoomUI; // <-- lobbyUI 대신 ChatRoomUI으로 변경!
    QPushButton* m_enterChatRoomButton; // 입장/나가기 겸용 버튼

    QListWidget* m_roomListWidget;
    QPushButton* m_refreshRoomListButton;
    QLineEdit* m_createRoomNameLineEdit;
    QPushButton* m_createRoomButton;
};

#endif // LOBBYMAINUI_H
