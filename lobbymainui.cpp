#include "lobbymainui.h"
#include "ui_lobbymainui.h"
#include <QMessageBox> // QMessageBox 사용을 위해 추가
#include <QDebug> // qDebug() 사용을 위해 추가
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "roomlistui.h"

LobbyMainUI::LobbyMainUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LobbyMainUI)
    , m_clientChat(clientChat)
{
    // 0711 .cpp 파일과 .ui파일 분리
    ui->setupUi(this); // new 적고 setupUi까지 해줘야 메모리에 올라감

    // chatHandler 실질적인 handler 받아오려고 (그냥 new하면 가짜 객체에 불과)
    m_chatHandler = m_clientChat->getChatHandler(); // 실질적인 handler 받아오기
    connect(m_chatHandler, &ChatHandler::addRoomResult, this, &LobbyMainUI::handleRoomCreationResult); // 미리 해보기

    m_roomListUI = new RoomListUI(m_clientChat, this);
    m_chatRoomUI = new ChatRoomUI(m_clientChat, this);

    // 스택 위젯에 UI 페이지 추가
    ui->rightStackedWidget->addWidget(m_roomListUI);
    ui->rightStackedWidget->addWidget(m_chatRoomUI);
    // 찻 화면은 룸리스트 보여주는 거
    ui->rightStackedWidget->setCurrentIndex(0);

    // 하위에서 버튼 누르면 상위에서 페이지 바뀌어야 하는데 객체 책임을 위해 분리
    connect(m_roomListUI, &RoomListUI::requestPageChange, this, &LobbyMainUI::changePage);
    connect(m_chatRoomUI, &ChatRoomUI::requestLeaveRoom, this, &LobbyMainUI::on_exitChatRoomButton_clicked);

}

LobbyMainUI::~LobbyMainUI()
{
    // 자식 위젯들은 부모 위젯 (QSplitter, QStackedWidget 등)이 소멸될 때 함께 소멸
}

void LobbyMainUI::changePage(int index)
{
    ui->rightStackedWidget->setCurrentIndex(index);
}

// 로비 UI가 활성화될 때 호출될 초기화 함수 구현
void LobbyMainUI::initializeLobby()
{
    qDebug() << "LobbyMainUI::initializeLobby() 호출: 채팅방 목록 요청";
    requestRoomList(); // 로비 화면이 준비되면 채팅방 목록 요청
}

// 왼쪽 패널 슬롯
void LobbyMainUI::on_goToShopButton_clicked()
{
    qDebug() << "상점 가기 버튼 클릭";
    ui->leftStackedWidget->setCurrentIndex(1); // 상점 위젯으로 전환
}

void LobbyMainUI::on_goToMapButton_clicked()
{
    qDebug() << "맵으로 돌아가기 버튼 클릭";
    ui->leftStackedWidget->setCurrentIndex(0); // 맵 위젯으로 전환
}

// 채팅방 목록 및 생성 관련 슬롯 구현
// 서버에 채팅방 목록 요청
void LobbyMainUI::requestRoomList()
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

/*
// 서버로부터 받은 채팅방 목록으로 UI 업데이트
void LobbyMainUI::updateRoomList(const QJsonArray &roomList)
{
    qDebug() << "채팅방 목록 수신: " << roomList;
    m_roomListWidget->clear(); // 기존 목록 지우기
    for(const QJsonValue& value : roomList){
        if(value.isObject()){
            QJsonObject roomObj = value.toObject();
            QString roomName = roomObj["name"].toString();
            int memberCount = roomObj["cnt"].toInt();
            m_roomListWidget->addItem(QString("%1 (%2명)").arg(roomName).arg(memberCount));
        }
    }
}
*/

/*
// 채팅방 생성 버튼 클릭 시
void LobbyMainUI::on_createChatRoomButton_clicked()
{
    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }
    QString roomName = ui->createRoomNameLineEdit->text().trimmed();
    if (roomName.isEmpty()) {
        QMessageBox::warning(this, "채팅방 생성", "채팅방 이름을 입력해주세요.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "add_r";
    obj["rName"] = roomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 생성 요청 전송: " << doc.toJson(QJsonDocument::Compact);
}
*/


// 방 생성 결과 처리 슬롯
void LobbyMainUI::handleRoomCreationResult(bool success, const QString& message)
{
    if (success) {
        //QMessageBox::information(this, "채팅방 생성", message);
        QString createdRoomName = m_roomListUI->ui->createRoomNameLineEdit->text().trimmed();
        m_roomListUI->ui->createRoomNameLineEdit->clear();

        //requestRoomList(); // 방 생성 후 목록 갱신 요청
        sendJoinRoomRequest(createdRoomName); // 방 생성 성공 시 바로 입장 요청
    } else {
        //QMessageBox::warning(this, "채팅방 생성 실패", message);
    }
}


// 특정 방 이름으로 입장 요청을 보냄
void LobbyMainUI::sendJoinRoomRequest(const QString& roomName)
{
    qDebug() << "sendJoinRoomRequest 호출: 방 입장 요청 - " << roomName;

    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "join_r";
    obj["rName"] = roomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 입장 요청 전송: " << doc.toJson(QJsonDocument::Compact);
}

/*
// 방 입장 버튼 클릭 시 (오직 입장 기능만 수행)
void LobbyMainUI::on_enterChatRoomButton_clicked()
{
    qDebug() << "채팅방 입장 버튼 클릭";

    QListWidgetItem* selectedItem = m_roomListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "채팅방 입장", "입장할 채팅방을 선택해주세요.");
        return;
    }
    QString fullText = selectedItem->text();
    QString roomName = fullText.left(fullText.indexOf(" ("));

    sendJoinRoomRequest(roomName);
}
*/


// 방 입장 결과 처리 슬롯
void LobbyMainUI::handleRoomJoinResult(bool success, const QString& message, const QString& roomName)
{
    qDebug() << "[Client LobbyMainUI] handleRoomJoinResult 호출됨. 결과: 성공=" << success << ", 메시지=" << message << ", 방 이름: " << roomName;

    if (success) {
        QMessageBox::information(this, "채팅방 입장", message);
        ui->rightStackedWidget->setCurrentIndex(1); // ChatRoomUI 화면으로 전환
        qDebug() << "[Client LobbyMainUI] 오른쪽 스택 위젯 인덱스 1 (ChatRoomUI)으로 전환됨.";

        // 채팅방 목록 관련 UI 요소 숨김/비활성화
        //ui->refreshRoomListButton->setVisible(false);
        //ui->createRoomNameLineEdit->setVisible(false);
        //ui->createRoomButton->setVisible(false);
        //m_roomListWidget->setVisible(false);

        // ChatRoomUI에 방 이름 설정
        m_chatRoomUI->setRoomName(roomName);
        //qDebug() << "[Client LobbyMainUI] ChatRoomUI에 방 이름 설정: " << roomName;

        // ChatRoomUI의 채팅창에 입장 메시지 추가
        m_chatRoomUI->appendChatMessage(QString("[시스템] '%1' 방에 입장했습니다.").arg(roomName));

    } else {
        QMessageBox::warning(this, "채팅방 입장 실패", message);
        qDebug() << "[Client LobbyMainUI] 방 입장 실패.";
    }
}


// 방 나가기 버튼 클릭 시 (ChatRoomUI의 "목록으로 돌아가기" 버튼이 이 함수를 호출)
void LobbyMainUI::on_exitChatRoomButton_clicked()
{
    qDebug() << "채팅방 나가기 버튼 클릭";
    QString currentRoomName = "없음";
    if (m_chatRoomUI && m_chatRoomUI->m_currentRoomNameLabel) {
        currentRoomName = m_chatRoomUI->m_currentRoomNameLabel->text().replace("현재 방: ", "");
    }

    if (currentRoomName.isEmpty() || currentRoomName == "없음") {
        QMessageBox::warning(this, "오류", "현재 입장한 채팅방이 없습니다.");
        return;
    }

    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "leave_r";
    obj["rName"] = currentRoomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 나가기 요청 전송: " << doc.toJson(QJsonDocument::Compact);
}


/*
// 방 나가기 결과 처리 슬롯
void LobbyMainUI::handleRoomLeaveResult(bool success, const QString& message)
{
    qDebug() << "[Client LobbyMainUI] handleRoomLeaveResult 호출됨. 성공: " << success << ", 메시지: " << message;
    if (success) {
        QMessageBox::information(this, "채팅방 나가기", message);
        ui->rightStackedWidget->setCurrentIndex(0); // 채팅방 목록 위젯으로 전환
        qDebug() << "[Client LobbyMainUI] 오른쪽 스택 위젯 인덱스 0 (채팅방 목록)으로 전환됨.";

        // 채팅방 목록 관련 UI 요소 다시 표시/활성화
        ui->refreshRoomListButton->setVisible(true);
        ui->createRoomNameLineEdit->setVisible(true);
        ui->createRoomButton->setVisible(true);
        ui->roomListWidget->setVisible(true);

        requestRoomList(); // 방 목록 새로고침
        ui->currentChatRoomUI->setRoomName("없음"); // ChatRoomUI의 방 이름 초기화
        ui->currentChatRoomUI->appendChatMessage("[시스템] 채팅방을 나갔습니다."); // ChatRoomUI의 채팅창 초기화 메시지
    } else {
        QMessageBox::warning(this, "채팅방 나가기 실패", message);
        qDebug() << "[Client LobbyMainUI] 방 나가기 실패.";
    }
}
*/

