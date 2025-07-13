#include "roomdisplayui.h"
#include "ui_roomdisplayui.h"

RoomDisplayUI::RoomDisplayUI(ClientChat* clientChat,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomDisplayUI)
{
    ui->setupUi(this);
}

RoomDisplayUI::~RoomDisplayUI()
{
    delete ui;
}
