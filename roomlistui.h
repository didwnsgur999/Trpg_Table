#ifndef ROOMLISTUI_H
#define ROOMLISTUI_H

#include <QWidget>
#include <QJsonArray>
#include "clientchat.h"
#include <QListWidgetItem>
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
    void requestRoomList();

public slots:
    void handleRoomCreationResult(bool success, const QString& message);
    void handleRoomJoinResult(bool success, const QString& roomName);
    void updateRoomList(const QJsonArray& roomList);

private slots:
    void on_enterRoomButton_clicked();
    void on_createRoomButton_clicked();

    void on_resetRoomListButton_clicked();

    void on_chatListWidget_itemClicked(QListWidgetItem *item);

signals:
    void createRoomRequested(const QString& roomName);
    void joinRoomRequested(const QString& roomName);

private:
    Ui::RoomListUI *ui;
    ClientChat* m_clientChat;
};

#endif // ROOMLISTUI_H
