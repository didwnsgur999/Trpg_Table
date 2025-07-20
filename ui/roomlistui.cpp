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
    connect(m_clientChat->getChatHandler(),&ChatHandler::roomListReceived,this,&RoomListUI::updateRoomList);
    connect(m_clientChat->getChatHandler(),&ChatHandler::joinRoomResult,this,&RoomListUI::handleRoomJoinResult);
}

RoomListUI::~RoomListUI()
{
    delete ui;
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
//=======================//
// 서버에 채팅방 목록 요청
//=======================//
//reset room clicked
void RoomListUI::on_resetRoomListButton_clicked()
{
    requestRoomList();
}

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
//룸리스트 초기화
void RoomListUI::updateRoomList(const QJsonArray &roomList)
{
    ui->chatListWidget->clear();
    for (const auto& val : roomList) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();
            QString roomName = obj["name"].toString();
            int memberCount = obj["cnt"].toInt();
            QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2명)").arg(roomName).arg(memberCount));
            item->setData(Qt::UserRole,roomName);
            ui->chatListWidget->addItem(item);
        }
    }
}

//룸 생성
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

//join쪽 코드 입장 버튼 누르면 보낸다.
void RoomListUI::on_enterRoomButton_clicked()
{
    // 서버에 입장 신호 보냄.
    QString roomName = ui->createRoomLineEdit->text();
    QJsonObject obj;
    obj["cmd"] = "join_r";
    obj["rName"] = roomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}

void RoomListUI::handleRoomJoinResult(bool success, const QString &roomName)
{
    if(success){
        Backend::getInstance().setRoom(roomName);
        qDebug()<<Backend::getInstance().getRoom();
        emit joinRoomRequested(roomName);
    }else{
        QMessageBox::warning(this,tr("입장 실패"),tr("당신은 방에 들어가지 못했습니다."));
    }
}

//item clicked -> text to ui->chat
void RoomListUI::on_chatListWidget_itemClicked(QListWidgetItem *item)
{
    //roomName 가져와서 createRoomLineEdit에 roomName 처리.
    QString roomName = item->data(Qt::UserRole).toString();
    ui->createRoomLineEdit->setText(roomName);
}

