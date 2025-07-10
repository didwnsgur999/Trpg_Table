#include "lobbymainui.h"
#include <QMessageBox> // QMessageBox 사용을 위해 추가
#include <QDebug> // qDebug() 사용을 위해 추가
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

LobbyMainUI::LobbyMainUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , m_clientChat(clientChat)
{
    // 메인 Splitter 생성 (2:1 분할 담당)
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // 왼쪽 패널 (맵/상점) 설정
    m_leftPaneWidget = new QWidget(m_mainSplitter);
    QVBoxLayout* leftLayout = new QVBoxLayout(m_leftPaneWidget);

    m_leftStackedWidget = new QStackedWidget(m_leftPaneWidget);

    // 맵 표시용 임시 위젯 (0번 인덱스)
    m_mapDisplayWidget = new QWidget(m_leftStackedWidget);
    QVBoxLayout* mapLayout = new QVBoxLayout(m_mapDisplayWidget);
    QLabel* mapLabel = new QLabel("~~여기는 맵 화면~~", m_mapDisplayWidget);
    mapLabel->setAlignment(Qt::AlignCenter);
    mapLayout->addWidget(mapLabel);

    m_leftShopButton = new QPushButton("상점 가기", m_mapDisplayWidget);
    connect(m_leftShopButton, &QPushButton::clicked, this, &LobbyMainUI::on_goToShopButton_clicked);
    mapLayout->addWidget(m_leftShopButton);
    m_leftStackedWidget->addWidget(m_mapDisplayWidget); // 0번 인덱스

    // 상점 표시용 임시 위젯 (1번 인덱스)
    m_shopWidget = new QWidget(m_leftStackedWidget);
    QVBoxLayout* shopLayout = new QVBoxLayout(m_shopWidget);
    QLabel* shopLabel = new QLabel("~~상점 화면~~", m_shopWidget);
    shopLabel->setAlignment(Qt::AlignCenter);
    shopLayout->addWidget(shopLabel);

    m_leftMapButton = new QPushButton("맵으로 돌아가기", m_shopWidget);
    connect(m_leftMapButton, &QPushButton::clicked, this, &LobbyMainUI::on_goToMapButton_clicked);
    shopLayout->addWidget(m_leftMapButton);
    m_leftStackedWidget->addWidget(m_shopWidget); // 1번 인덱스

    leftLayout->addWidget(m_leftStackedWidget);
    m_leftPaneWidget->setLayout(leftLayout); // 레이아웃 설정

    // 오른쪽 패널 (채팅) 설정
    m_rightPaneWidget = new QWidget(m_mainSplitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rightPaneWidget);

    // 상단: 환영합니다! 라벨
    m_welcomeLabel = new QLabel("환영합니다!", m_rightPaneWidget); // 여기에 실제 사용자 이름을 받을 수 있도록 추후 수정해야하지 않을까
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(m_welcomeLabel);

    m_rightStackedWidget = new QStackedWidget(m_rightPaneWidget);

    // 채팅방 목록 위젯 (0번 인덱스)
    m_chatRoomListWidget = new QWidget(m_rightStackedWidget);
    QVBoxLayout* chatListLayout = new QVBoxLayout(m_chatRoomListWidget);
    chatListLayout->addWidget(new QLabel(" --- 현재 채팅방 목록 --- ", m_chatRoomListWidget));

    // 채팅방 목록 QListWidget 추가
    m_roomListWidget = new QListWidget(m_chatRoomListWidget);
    chatListLayout->addWidget(m_roomListWidget);

    // 채팅방 목록 새로고침 버튼
    m_refreshRoomListButton = new QPushButton("새로고침", m_chatRoomListWidget); // 부모 위젯 지정
    connect(m_refreshRoomListButton, &QPushButton::clicked, this, &LobbyMainUI::requestRoomList);
    chatListLayout->addWidget(m_refreshRoomListButton);

    // 채팅방 생성 UI (입력 필드 + 버튼)
    QHBoxLayout* createRoomLayout = new QHBoxLayout();
    m_createRoomNameLineEdit = new QLineEdit(m_chatRoomListWidget);
    m_createRoomNameLineEdit->setPlaceholderText("새 채팅방 이름 입력");
    m_createRoomButton = new QPushButton("채팅방 만들기", m_chatRoomListWidget);
    connect(m_createRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_createChatRoomButton_clicked);
    createRoomLayout->addWidget(m_createRoomNameLineEdit);
    createRoomLayout->addWidget(m_createRoomButton);
    chatListLayout->addLayout(createRoomLayout);

    // 채팅방 입장 버튼 (오직 입장 기능만 수행)
    m_enterChatRoomButton = new QPushButton("선택한 채팅방 입장", m_chatRoomListWidget);
    connect(m_enterChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_enterChatRoomButton_clicked);
    chatListLayout->addWidget(m_enterChatRoomButton);

    m_rightStackedWidget->addWidget(m_chatRoomListWidget); // 0번 인덱스 (채팅방 목록)

    // 새로운 ChatRoomUI를 채팅방 상세 화면으로 활용 (1번 인덱스)
    m_currentChatRoomUI = new ChatRoomUI(m_clientChat, m_rightStackedWidget);
    m_rightStackedWidget->addWidget(m_currentChatRoomUI); // 1번 인덱스 (새로운 ChatRoomUI)

    // ChatRoomUI에서 방 나가기 요청 시그널 연결 (새로 추가)
    connect(m_currentChatRoomUI, &ChatRoomUI::requestLeaveRoom, this, &LobbyMainUI::on_exitChatRoomButton_clicked);

    rightLayout->addWidget(m_rightStackedWidget);
    m_rightPaneWidget->setLayout(rightLayout); // 레이아웃 설정

    // Splitter에 패널 추가 및 2:1 비율 설정
    QList<int> initialSizes;
    initialSizes << 500 << 250; // 초기 크기 비율 (예시)
    m_mainSplitter->setSizes(initialSizes);
    m_mainSplitter->addWidget(m_leftPaneWidget);
    m_mainSplitter->addWidget(m_rightPaneWidget);


    // LobbyMainUI의 메인 레이아웃에 Splitter 추가
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
    setLayout(mainLayout);

    connect(m_clientChat->getChatHandler(), &ChatHandler::roomListReceived, this, &LobbyMainUI::updateRoomList);
    connect(m_clientChat->getChatHandler(), &ChatHandler::addRoomResult, this, &LobbyMainUI::handleRoomCreationResult);
    connect(m_clientChat->getChatHandler(), &ChatHandler::joinRoomResult, this,
            static_cast<void (LobbyMainUI::*)(bool, const QString&, const QString&)>(&LobbyMainUI::handleRoomJoinResult));
    connect(m_clientChat->getChatHandler(), &ChatHandler::leaveRoomResult, this, &LobbyMainUI::handleRoomLeaveResult);
}

LobbyMainUI::~LobbyMainUI()
{
    // 자식 위젯들은 부모 위젯 (QSplitter, QStackedWidget 등)이 소멸될 때 함께 소멸
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
    m_leftStackedWidget->setCurrentIndex(1); // 상점 위젯으로 전환
}

void LobbyMainUI::on_goToMapButton_clicked()
{
    qDebug() << "맵으로 돌아가기 버튼 클릭";
    m_leftStackedWidget->setCurrentIndex(0); // 맵 위젯으로 전환
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

// 채팅방 생성 버튼 클릭 시
void LobbyMainUI::on_createChatRoomButton_clicked()
{
    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }
    QString roomName = m_createRoomNameLineEdit->text().trimmed();
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

// 방 생성 결과 처리 슬롯
void LobbyMainUI::handleRoomCreationResult(bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, "채팅방 생성", message);
        QString createdRoomName = m_createRoomNameLineEdit->text().trimmed();
        m_createRoomNameLineEdit->clear();

        requestRoomList(); // 방 생성 후 목록 갱신 요청
        sendJoinRoomRequest(createdRoomName); // 방 생성 성공 시 바로 입장 요청
    } else {
        QMessageBox::warning(this, "채팅방 생성 실패", message);
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

// 방 입장 결과 처리 슬롯
void LobbyMainUI::handleRoomJoinResult(bool success, const QString& message, const QString& roomName)
{
    qDebug() << "[Client LobbyMainUI] handleRoomJoinResult 호출됨. 결과: 성공=" << success << ", 메시지=" << message << ", 방 이름: " << roomName;

    if (success) {
        QMessageBox::information(this, "채팅방 입장", message);
        m_rightStackedWidget->setCurrentIndex(1); // ChatRoomUI 화면으로 전환
        qDebug() << "[Client LobbyMainUI] 오른쪽 스택 위젯 인덱스 1 (ChatRoomUI)으로 전환됨.";

        // 채팅방 목록 관련 UI 요소 숨김/비활성화
        m_refreshRoomListButton->setVisible(false);
        m_createRoomNameLineEdit->setVisible(false);
        m_createRoomButton->setVisible(false);
        m_roomListWidget->setVisible(false);

        // ChatRoomUI에 방 이름 설정
        m_currentChatRoomUI->setRoomName(roomName);
        qDebug() << "[Client LobbyMainUI] ChatRoomUI에 방 이름 설정: " << roomName;

        // ChatRoomUI의 채팅창에 입장 메시지 추가
        m_currentChatRoomUI->appendChatMessage(QString("[시스템] '%1' 방에 입장했습니다.").arg(roomName));

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
    if (m_currentChatRoomUI && m_currentChatRoomUI->m_currentRoomNameLabel) {
        currentRoomName = m_currentChatRoomUI->m_currentRoomNameLabel->text().replace("현재 방: ", "");
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

// 방 나가기 결과 처리 슬롯
void LobbyMainUI::handleRoomLeaveResult(bool success, const QString& message)
{
    qDebug() << "[Client LobbyMainUI] handleRoomLeaveResult 호출됨. 성공: " << success << ", 메시지: " << message;
    if (success) {
        QMessageBox::information(this, "채팅방 나가기", message);
        m_rightStackedWidget->setCurrentIndex(0); // 채팅방 목록 위젯으로 전환
        qDebug() << "[Client LobbyMainUI] 오른쪽 스택 위젯 인덱스 0 (채팅방 목록)으로 전환됨.";

        // 채팅방 목록 관련 UI 요소 다시 표시/활성화
        m_refreshRoomListButton->setVisible(true);
        m_createRoomNameLineEdit->setVisible(true);
        m_createRoomButton->setVisible(true);
        m_roomListWidget->setVisible(true);

        requestRoomList(); // 방 목록 새로고침
        m_currentChatRoomUI->setRoomName("없음"); // ChatRoomUI의 방 이름 초기화
        m_currentChatRoomUI->appendChatMessage("[시스템] 채팅방을 나갔습니다."); // ChatRoomUI의 채팅창 초기화 메시지
    } else {
        QMessageBox::warning(this, "채팅방 나가기 실패", message);
        qDebug() << "[Client LobbyMainUI] 방 나가기 실패.";
    }
}
