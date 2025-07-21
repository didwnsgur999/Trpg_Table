#ifndef ROOMDISPLAYUI_H
#define ROOMDISPLAYUI_H

#include <QWidget>
#include "mycore/clientchat.h"
#include <QGraphicsScene>
#include <QStringListModel>
#include <QListWidgetItem>

namespace Ui {
class RoomDisplayUI;
}

class RoomDisplayUI : public QWidget
{
    Q_OBJECT

public:
    explicit RoomDisplayUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~RoomDisplayUI();
    void loadProductList();
    void addRoomItemHandle(const QJsonObject& product);
    void loadRoomItemList();
    void delRoomItemHandle(const QJsonObject& item);
    void movRoomItemHandle(int id, int newx,int newy, int z);
    void movRoomItemServerHandle(const QJsonObject& item);
    void displayItem(int iid);

signals:
    void requestPageChange(int index);

private slots:
    void on_ItemButton_clicked();

    void on_UserItemListWidget_itemDoubleClicked(QListWidgetItem *item);

    //void on_RoomItemListWidget_itemClicked(QListWidgetItem *item);

    void on_RoomItemListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::RoomDisplayUI *ui;
    ClientChat* m_clientChat;
    QGraphicsScene* scene;
    QHash<QString, QGraphicsPixmapItem*> imageItems;
};

#endif // ROOMDISPLAYUI_H
