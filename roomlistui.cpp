#include <QMessageBox>

#include "roomlistui.h"
#include "ui_roomlistui.h"
#include "clientchat.h"
#include "Backend.h"

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

    emit joinRoomRequested(roomName);
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

    ui->createRoomLineEdit->clear();
    QJsonObject obj;
    obj["cmd"] = "add_r";
    obj["rName"] = roomName;
    //===================//
    //for debug only
    //===================//
    Backend::getInstance().setRoom(roomName);
    qDebug()<<Backend::getInstance().getRoom();
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    emit createRoomRequested(roomName);
}

// 서버에 채팅방 목록 요청
void RoomListUI::requestRoomList()
{
    if(!m_clientChat->isConnected()){
        QMessageBox::warning(this, "오류", "서버 미연결로 채팅방 목록 가져올 수 없습니다!");
        return;
    }
    QJsonObject obj;
    obj["cmd"] = "list_r"; // 서버 프로토콜에 맞게 "list_r" 사용
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 목록 요청 전송: " << doc.toJson(QJsonDocument::Compact);
}

void RoomListUI::handleRoomCreationResult(bool success, const QString &message)
{
    if(success){
        QString roomName = ui->createRoomLineEdit->text().trimmed();
        Backend::getInstance().setRoom(roomName);
        qDebug()<<Backend::getInstance().getRoom();
        ui->createRoomLineEdit->clear();

        emit joinRoomRequested(roomName); // 그 이름 갖고 자동으로 입장
    } else{
        qDebug() << "설마 안만들어짐??";
    }
}

void RoomListUI::handleRoomJoinResult(bool success, const QString &message, const QString &roomName)
{
    // 일단 아직 목록 누르는 거 미구현
}

void RoomListUI::updateRoomList(const QJsonArray &roomList)
{
    ui->chatListWidget->clear();
    for (const auto& val : roomList) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();
            QString roomName = obj["name"].toString();
            int memberCount = obj["cnt"].toInt();
            ui->chatListWidget->addItem(QString("%1 (%2명)").arg(roomName).arg(memberCount));
        }
    }
}
