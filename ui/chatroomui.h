#ifndef CHATROOMUI_H
#define CHATROOMUI_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "mycore/clientchat.h" // ClientChat을 사용하기 위해 포함
#include <QListWidgetItem>

class ClientChat; // 전방 선언

namespace Ui {
class ChatRoomUI;
}

class ChatRoomUI : public QWidget
{
    Q_OBJECT

public:
    explicit ChatRoomUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~ChatRoomUI();

    void setRoomName(const QString& name); // 현재 방 이름 설정
    void appendChatMessage(const QString& message); // 채팅 메시지 추가
    void requestUserList();
    void requestRoomUserList();
    void loadAllUserList(const QJsonArray& UserList);
    void loadRoomUserList(const QJsonArray& RoomUserList);
    void InviteHandle(const QString& rName);
    void banFailHandle(const QString& message);
    QLabel* m_currentRoomNameLabel; // public으로 유지


signals:
    void requestLeaveRoom(); // 방 나가기 요청 시그널 (LobbyMainUI로 전달)

private slots:
    void on_sendButton_clicked(); // 메시지 전송 버튼 클릭 시
    void on_chatreceived(const QString& msg); // 채팅 메시지 수신 시
    void on_backToListButton_clicked(); // "목록으로 돌아가기" 버튼 클릭 시 슬롯
    void handleRoomLeaveResult(bool success, const QString& message);

    void on_UserListButton_clicked();

    void on_UserListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_RoomUserListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::ChatRoomUI *ui;
    ClientChat* m_clientChat;
};

#endif // CHATROOMUI_H
