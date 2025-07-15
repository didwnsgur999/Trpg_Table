#ifndef ROOMDISPLAYUI_H
#define ROOMDISPLAYUI_H

#include <QWidget>
#include "clientchat.h"
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

signals:
    void requestPageChange(int index);

private slots:
    void on_ItemButton_clicked();

    void on_ItemListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::RoomDisplayUI *ui;
    ClientChat* m_clientChat;
    QGraphicsScene* scene;
    QHash<QString, QGraphicsPixmapItem*> imageItems;
};

#endif // ROOMDISPLAYUI_H
