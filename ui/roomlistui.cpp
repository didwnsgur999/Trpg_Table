#include <QMessageBox>

#include "roomlistui.h"
#include "ui_roomlistui.h"
#include "mycore/clientchat.h"
#include "mycore/Backend.h"

RoomListUI::RoomListUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomListUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this);
    connect(m_clientChat->getChatHandler(),&ChatHandler::roomListReceived,this,&RoomListUI::updateRoomList);
    //RoomListUI에서 joinRoomResult 처리
    connect(m_clientChat->getChatHandler(),&ChatHandler::joinRoomResult,
            this,&RoomListUI::handleRoomJoinResult);
    connect(m_clientChat->getChatHandler(),&ChatHandler::inviteFailed,this,&RoomListUI::InviteFailHandle);
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

//join 코드 입장 버튼 누르면 보낸다.
void RoomListUI::on_enterRoomButton_clicked()
{
    // 서버에 입장 신호 보냄.
    QString roomName = ui->createRoomLineEdit->text();
    QJsonObject obj;
    // 미리 예약된 cmd
    obj["cmd"] = "join_r";
    obj["rName"] = roomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}
//입장 성공 실패 처리
void RoomListUI::handleRoomJoinResult(bool success, const QString &roomName)
{
    if(success){
        Backend::getInstance().setRoom(roomName);
        emit joinRoomRequested(roomName);
    }else{
        QMessageBox::warning(this,tr("Join Failed"),tr("you've failed to join the room"));
    }
}

//방 이름 누르면 자동으로 textline에 설정됨.
void RoomListUI::on_chatListWidget_itemClicked(QListWidgetItem *item)
{
    //roomName 가져와서 createRoomLineEdit에 roomName 처리.
    QString roomName = item->data(Qt::UserRole).toString();
    ui->createRoomLineEdit->setText(roomName);
}
//초대 수락
void RoomListUI::InviteHandle(const QString& rName){
    ui->createRoomLineEdit->setText(rName);
    on_enterRoomButton_clicked();
}
//초대 실패
void RoomListUI::InviteFailHandle(){
    QMessageBox::warning(this,tr("Invite Failed"),tr("The opponent is currently offline"));
}

