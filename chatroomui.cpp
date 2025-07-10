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
    qDebug() << "[Client ChatRoomUI] 생성자 호출 시작.";
    ui->setupUi(this); // UI 설정

    // 현재 방 이름을 표시할 라벨 생성 및 추가
    m_currentRoomNameLabel = new QLabel("현재 방: 없음", this);
    m_currentRoomNameLabel->setAlignment(Qt::AlignCenter);

    // ui->setupUi가 설정한 레이아웃을 가져와서 m_currentRoomNameLabel을 추가
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertWidget(0, m_currentRoomNameLabel); // 맨 위에 삽입
        qDebug() << "[Client ChatRoomUI] m_currentRoomNameLabel을 기존 레이아웃에 성공적으로 추가했습니다.";
    } else {
        qDebug() << "[Client ChatRoomUI] 경고: ChatRoomUI에 기존 레이아웃이 없습니다. 수동으로 레이아웃을 설정합니다.";
        // Fallback: 레이아웃이 없는 경우 수동으로 설정
        mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(m_currentRoomNameLabel);
        mainLayout->addWidget(ui->chatTextEdit);
        mainLayout->addLayout(ui->horizontalLayout); // ui 파일의 horizontalLayout을 추가
        mainLayout->addWidget(ui->backToListButton); // 목록으로 돌아가기 버튼도 레이아웃에 추가
        setLayout(mainLayout);
    }

    // ChatHandler로부터 채팅 메시지 수신 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::chatreceived, this, &ChatRoomUI::on_chatreceived);

    qDebug() << "[Client ChatRoomUI] 생성자 호출 종료.";
}

ChatRoomUI::~ChatRoomUI()
{
    delete ui;
}

void ChatRoomUI::setRoomName(const QString &name)
{
    if (m_currentRoomNameLabel) {
        m_currentRoomNameLabel->setText("현재 방: " + name);
        qDebug() << "[Client ChatRoomUI] 현재 방 이름 설정: " << name;
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
    obj["rName"] = m_currentRoomNameLabel->text().replace("현재 방: ", ""); // 현재 방 이름 가져오기

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
    QString currentRoomName = m_currentRoomNameLabel->text().replace("현재 방: ", "");
    emit requestLeaveRoom(currentRoomName); // LobbyMainUI로 방 나가기 요청 시그널 방출
    m_rightStackedWidget->setCurrentIndex(0);
}
