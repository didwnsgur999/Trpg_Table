#include "chatroomui.h"
#include "ui_chatroomui.h" // ui_chatroomui.h 헤더 파일 포함
#include <QJsonObject>
#include <QJsonDocument>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include "mycore/Backend.h"

ChatRoomUI::ChatRoomUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatRoomUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this); // UI 설정

    // ChatHandler로부터 채팅 메시지 수신 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::chatreceived, this, &ChatRoomUI::on_chatreceived);
    // 챗룸유아이에서 나가는 것도
    connect(m_clientChat->getChatHandler(), &ChatHandler::leaveRoomResult, this, &ChatRoomUI::handleRoomLeaveResult);
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
// 새로 추가된 "목록으로 돌아가기" 버튼 클릭 슬롯
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
        Backend::getInstance().setRoom(NULL);
        ui->messageLineEdit->clear();
        ui->chatTextEdit->clear();

        emit requestLeaveRoom();
    } else {
        qDebug("Failed to go out");
    }
}
