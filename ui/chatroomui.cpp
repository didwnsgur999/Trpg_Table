#include "chatroomui.h"
#include "ui_chatroomui.h" // ui_chatroomui.h 헤더 파일 포함
#include <QJsonObject>
#include <QJsonDocument>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include "mycore/Backend.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

ChatRoomUI::ChatRoomUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatRoomUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this); // UI 설정

    ui->RoomUserListWidget->setMaximumWidth(0);
    ui->UserListWidget->setMaximumWidth(0);

    // ChatHandler로부터 채팅 메시지 수신 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::chatreceived, this, &ChatRoomUI::on_chatreceived);
    // 챗룸유아이에서 나가는 것도
    connect(m_clientChat->getChatHandler(), &ChatHandler::leaveRoomResult, this, &ChatRoomUI::handleRoomLeaveResult);
    connect(m_clientChat->getChatHandler(),&ChatHandler::roomUserListReceived, this,&ChatRoomUI::loadRoomUserList);
    connect(m_clientChat->getChatHandler(),&ChatHandler::AllUserListReceived, this,&ChatRoomUI::loadAllUserList);
    connect(m_clientChat->getChatHandler(),&ChatHandler::banreceived,this,&ChatRoomUI::banFailHandle);
}

ChatRoomUI::~ChatRoomUI()
{
    delete ui;
}
//===================//
//방 입장시 sequence
//===================//
void ChatRoomUI::setRoomName(const QString &name)
{
    if (ui->currentRoomNameLabel) {
        ui->currentRoomNameLabel->setText("현재 방: " + name);
    }
}

//===================//
//채팅 보내고 받을때 sequence
//===================//
void ChatRoomUI::appendChatMessage(const QString &message)
{
    if (ui && ui->chatTextEdit) {
        ui->chatTextEdit->append(message);
        qDebug() << "[Client ChatRoomUI] 채팅 메시지 추가: " << message;
    }
}
void ChatRoomUI::on_sendButton_clicked()
{
    QString message = ui->messageLineEdit->text();
    if (message.isEmpty()) {
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "chat";
    obj["text"] = message;
    obj["rName"] = Backend::getInstance().getRoom(); // 현재 방 이름 가져오기

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);

    ui->messageLineEdit->clear();
    //appendChatMessage("나: " + message); // 내가 보낸 메시지도 바로 표시
}

void ChatRoomUI::on_chatreceived(const QString &msg)
{
    appendChatMessage(msg); // 서버로부터 받은 메시지 표시
}
// ========================================//
// "목록으로 돌아가기" 버튼 클릭 슬롯
// =-======================================//
void ChatRoomUI::on_backToListButton_clicked()
{
    QString currentRoomName = Backend::getInstance().getRoom();

    if(currentRoomName.isEmpty()){
        QMessageBox::warning(this, "오류", "현재 입장한 채팅방이 없습니다.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "leave_r";
    obj["rName"] = currentRoomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}

void ChatRoomUI::handleRoomLeaveResult(bool success, const QString &message)
{
    if(success){
        // 초기화
        setRoomName("없음");
        Backend::getInstance().setRoom(QString());
        ui->messageLineEdit->clear();
        ui->chatTextEdit->clear();

        emit requestLeaveRoom();
    } else {
        qDebug("Failed to go out");
    }
}

void ChatRoomUI::on_UserListButton_clicked()
{
    static bool isOpen = false;

    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

    QPropertyAnimation *anim1 = new QPropertyAnimation(ui->UserListWidget, "maximumWidth", this);
    anim1->setDuration(200);
    anim1->setEasingCurve(QEasingCurve::OutCubic);
    QPropertyAnimation *anim2 = new QPropertyAnimation(ui->RoomUserListWidget, "maximumWidth", this);
    anim2->setDuration(200);
    anim2->setEasingCurve(QEasingCurve::OutCubic);

    if (isOpen) {
        anim1->setStartValue(150);
        anim1->setEndValue(0);
        anim2->setStartValue(150);
        anim2->setEndValue(0);
        ui->UserListButton->setText("<<");
    } else {
        anim1->setStartValue(0);
        anim1->setEndValue(150);
        anim2->setStartValue(0);
        anim2->setEndValue(150);
        ui->UserListButton->setText(">>");
    }

    group->addAnimation(anim1);
    group->addAnimation(anim2);

    group->start();
    isOpen = !isOpen;
    if(isOpen) {
        //이거 열리면 Alluserlist랑 Roomlist 불러와야됨.
        requestUserList();
        requestRoomUserList();
    }
}
//User 가져오기.
void ChatRoomUI::requestUserList(){
    //유저 데이터 일체 받기.
    QJsonObject obj;
    obj["cmd"] = "list_users";

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}
//방 유저 가져오기.
void ChatRoomUI::requestRoomUserList(){
    QJsonObject obj;
    obj["cmd"] = "list_r_users";
    obj["rName"] = Backend::getInstance().getRoom();

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}
//가져온 전체 처리
void ChatRoomUI::loadAllUserList(const QJsonArray& UserList){
    ui->UserListWidget->clear();
    QListWidgetItem* labelItem = new QListWidgetItem("전체 접속 인원");
    labelItem->setFlags(Qt::NoItemFlags);
    labelItem->setTextAlignment(Qt::AlignCenter);
    ui->UserListWidget->addItem(labelItem);
    for (const auto& val : UserList) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();
            int id = obj["id"].toInt();
            QString Name = obj["Name"].toString();
            QListWidgetItem *item = new QListWidgetItem(QString("%1 (id: %2)").arg(Name).arg(id));
            item->setData(Qt::UserRole,id);
            ui->UserListWidget->addItem(item);
        }
    }
}
//가져온 방 인원 처리.
void ChatRoomUI::loadRoomUserList(const QJsonArray& RoomUserList){
    ui->RoomUserListWidget->clear();
    QListWidgetItem* labelItem = new QListWidgetItem("현재 방 인원");
    labelItem->setFlags(Qt::NoItemFlags);
    labelItem->setTextAlignment(Qt::AlignCenter);
    ui->RoomUserListWidget->addItem(labelItem);
    for (const auto& val : RoomUserList) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();
            int id = obj["id"].toInt();
            QString Name = obj["Name"].toString();
            QListWidgetItem *item = new QListWidgetItem(QString(tr("%1 (아이디: %2)")).arg(Name).arg(id));
            item->setData(Qt::UserRole,id);
            ui->RoomUserListWidget->addItem(item);
        }
    }
}
//방 밖에있는 사람 초대해야됨. 초대기능
void ChatRoomUI::on_UserListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();
    //이거 할때 RoomUserListWidget에 있으면 Qt::userRole중에 겹치는게있으면 return
    for(int i=0; i<ui->RoomUserListWidget->count(); i++){
        QListWidgetItem *roomUserItem = ui->RoomUserListWidget->item(i);
        int Userid = roomUserItem->data(Qt::UserRole).toInt();
        if(Userid==id){
            QMessageBox::warning(this,tr("초대불가"),tr("이미 유저리스트에 존재합니다. 만약 없다면 최신화 해주세요."));
            return;
        }
    }
    //초대장 보내기.
    QJsonObject obj;
    obj["cmd"] = "invite_r";
    obj["rName"] = Backend::getInstance().getRoom();
    obj["cid"] = id;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    QMessageBox::warning(this,tr("초대"),tr("%1 님에게 초대가 전송되었습니다.").arg(item->text()));
    //일단 방에서 나가고 들어가는 기능이 되야 처리가 가능하겠다.
    //아직 connect도 안됬고 이 함수만 만들어놨음
}
//초대 받았을때 방에 있으면 나가야됨
void ChatRoomUI::InviteHandle(const QString& rName){
    Q_UNUSED(rName);
    on_backToListButton_clicked();
}

//더블클릭 강퇴기능
void ChatRoomUI::on_RoomUserListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();
    if(id==Backend::getInstance().getUser()->getId()){
        QMessageBox::warning(this,tr("강퇴 거부"),tr("본인은 강퇴불가합니다."));
        return;
    }
    QJsonObject obj;
    obj["cmd"] = "ban_r_user";
    obj["rName"] = Backend::getInstance().getRoom();
    obj["mid"] = Backend::getInstance().getUser()->getId();
    obj["cid"] = id;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}

//실패시 실패이유 제공
void ChatRoomUI::banFailHandle(const QString& message){
    QMessageBox::warning(this,tr("%1").arg(message),tr("%1 : 이유로 강퇴처리 불가능합니다.").arg(message));
}
