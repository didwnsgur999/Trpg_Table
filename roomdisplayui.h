#ifndef ROOMDISPLAYUI_H
#define ROOMDISPLAYUI_H

#include <QWidget>
#include "clientchat.h"

namespace Ui {
class RoomDisplayUI;
}

class RoomDisplayUI : public QWidget
{
    Q_OBJECT

public:
    explicit RoomDisplayUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~RoomDisplayUI();

signals:
    void requestPageChange(int index);

private:
    Ui::RoomDisplayUI *ui;
};

#endif // ROOMDISPLAYUI_H
