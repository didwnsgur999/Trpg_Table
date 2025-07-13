#ifndef ROOMLISTUI_H
#define ROOMLISTUI_H

#include <QWidget>
#include "clientchat.h"

class ClientChat; // 전방 선언

namespace Ui {
class RoomListUI;
}

class RoomListUI : public QWidget
{
    Q_OBJECT

public:
    explicit RoomListUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~RoomListUI();

private slots:
    void on_enterRoomButton_clicked();

    void on_createRoomButton_clicked();

signals:
    void requestPageChange(int index);
    void sendJoinRoomRequest(const QString& roomName); // 일단 여기 선언하는데 실제 정의는 상위(로비)에 있음

private:
    Ui::RoomListUI *ui;
    ClientChat* m_clientChat;
};

#endif // ROOMLISTUI_H
