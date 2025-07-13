#include <QMessageBox>

#include "roomlistui.h"
#include "ui_roomlistui.h"
#include "clientchat.h"

#include "ui_lobbymainui.h"

RoomListUI::RoomListUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomListUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this);
}

RoomListUI::~RoomListUI()
{
    delete ui;
}


void RoomListUI::on_enterRoomButton_clicked()
{
    QListWidgetItem* selectedItem = ui->chatListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "채팅방 입장", "입장할 채팅방을 선택해주세요.");
        return;
    }
    QString fullText = selectedItem->text();
    QString roomName = fullText.left(fullText.indexOf(" ("));

    sendJoinRoomRequest(roomName);
}


void RoomListUI::on_createRoomButton_clicked()
{
    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }
    QString roomName = ui->createRoomLineEdit->text().trimmed();
    if (roomName.isEmpty()) {
        QMessageBox::warning(this, "채팅방 생성", "채팅방 이름을 입력해주세요.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "add_r";
    obj["rName"] = roomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    emit requestPageChange(1);
}

