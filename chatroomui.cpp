#include "chatroomui.h"
#include "ui_chatroomui.h" // ui_chatroomui.h 헤더 파일 포함
#include <QJsonObject>
#include <QJsonDocument>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

ChatRoomUI::ChatRoomUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatRoomUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this); // UI 설정

    // ChatHandler로부터 채팅 메시지 수신 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::chatreceived, this, &ChatRoomUI::on_chatreceived);
}

ChatRoomUI::~ChatRoomUI()
{
    delete ui;
}

void ChatRoomUI::setRoomName(const QString &name)
{
    qDebug() << "setRoomName찍히는 지 확인" << name;
    if (ui->currentRoomNameLabel) {
        ui->currentRoomNameLabel->setText("현재 방: " + name);
    }
}

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
    obj["rName"] = ui->currentRoomNameLabel->text().replace("현재 방: ", ""); // 현재 방 이름 가져오기

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);

    ui->messageLineEdit->clear();
    appendChatMessage("나: " + message); // 내가 보낸 메시지도 바로 표시
}

void ChatRoomUI::on_chatreceived(const QString &msg)
{
    appendChatMessage(msg); // 서버로부터 받은 메시지 표시
}

// 새로 추가된 "목록으로 돌아가기" 버튼 클릭 슬롯
void ChatRoomUI::on_backToListButton_clicked()
{
    qDebug() << "[Client ChatRoomUI] 목록으로 돌아가기 버튼 클릭.";
    QString currentRoomName = ui->currentRoomNameLabel->text().replace("현재 방: ", "");
    emit requestLeaveRoom(currentRoomName); // LobbyMainUI로 방 나가기 요청 시그널 방출
}
