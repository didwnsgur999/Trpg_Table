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
    m_welcomeLabel = new QLabel("환영합니다!", m_rightPaneWidget); // 여기에 실제 사용자 이름을 받을 수 있도록 추후 수정
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(m_welcomeLabel);
    // m_createChatRoomButton = new QPushButton("채팅방 생성", m_rightPaneWidget);
    // connect(m_createChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_createChatRoomButton_clicked);

    // rightLayout->addWidget(m_welcomeLabel);
    // rightLayout->addWidget(m_createChatRoomButton);

    m_rightStackedWidget = new QStackedWidget(m_rightPaneWidget);

    // 채팅방 목록 위젯 (0번 인덱스)
    m_chatRoomListWidget = new QWidget(m_rightStackedWidget);
    QVBoxLayout* chatListLayout = new QVBoxLayout(m_chatRoomListWidget);
    // QLabel* chatListLabel = new QLabel("~~채팅방 목록~~", m_chatRoomListWidget);
    // chatListLabel->setAlignment(Qt::AlignCenter);
    chatListLayout->addWidget(new QLabel(" --- 현재 채팅방 목록 --- ", m_chatRoomListWidget));

    // 채팅방 목록 QListWidget 추가
    m_roomListWidget = new QListWidget(m_chatRoomListWidget);
    chatListLayout->addWidget(m_roomListWidget);

    // 채팅방 목록 새로고침 버튼
    m_refreshRoomListButton = new QPushButton("새로고침", m_chatRoomListWidget); // 부모 위젯 지정
    // connect(m_refreshRoomListButton, &QPushButton::clicked, this, m_chatRoomListWidget);
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

    // 채팅방 입장 버튼
    m_enterChatRoomButton = new QPushButton("선택한 채팅방 입장", m_chatRoomListWidget);
    connect(m_enterChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_enterChatRoomButton_clicked);
    chatListLayout->addWidget(m_enterChatRoomButton);

    m_rightStackedWidget->addWidget(m_chatRoomListWidget); // 0번 인덱스 (채팅방 목록)

    // 기존 lobbyUI를 채팅방 상세 화면으로 활용 (1번 인덱스)
    m_currentChatRoomUI = new lobbyUI(m_clientChat, m_rightStackedWidget);
    // lobbyUI에서 '나가기' 시그널이 있다면 여기에 연결
    // connect(m_currentChatRoomUI, &lobbyUI::leaveChatRoom, this, &LobbyMainUI::on_exitChatRoomButton_clicked);
    m_rightStackedWidget->addWidget(m_currentChatRoomUI); // 1번 인덱스 (기존 lobbyUI)

    // 채팅방 나가기 버튼 (채팅방 상세 화면에 표시)
    m_exitChatRoomButton = new QPushButton("채팅방 나가기", m_currentChatRoomUI); // m_currentChatRoomUI의 자식으로
    m_exitChatRoomButton->setVisible(false); // 초기에는 숨김
    connect(m_exitChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_exitChatRoomButton_clicked);
    // m_currentChatRoomUI의 레이아웃에 m_exitChatRoomButton을 추가해야 함 (lobbyUI.ui 또는 lobbyUI.cpp에서)
    // 현재는 LobbyMainUI에서 직접 제어하므로, m_currentChatRoomUI의 레이아웃에 추가하는 로직이 필요.
    // 예: m_currentChatRoomUI->layout()->addWidget(m_exitChatRoomButton); (단, m_currentChatRoomUI에 레이아웃이 설정되어 있어야 함)
    // 더 나은 방법: m_currentChatRoomUI 내부에서 나가기 버튼을 관리하고 시그널을 통해 LobbyMainUI에 알림.

    rightLayout->addWidget(m_rightStackedWidget);
    m_rightPaneWidget->setLayout(rightLayout); // 레이아웃 설정

    // Splitter에 패널 추가 및 2:1 비율 설정
    m_mainSplitter->addWidget(m_leftPaneWidget);
    m_mainSplitter->addWidget(m_rightPaneWidget);

    // 초기 크기 비율 설정 (대략 2:1)
    QList<int> initialSizes;
    initialSizes << 500 << 250; // 초기 크기 비율 (예시)
    m_mainSplitter->setSizes(initialSizes);

    // LobbyMainUI의 메인 레이아웃에 Splitter 추가
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
    setLayout(mainLayout);

    // ChatHandler로부터 시그널 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::roomListReceived, this, &LobbyMainUI::updateRoomList);
    connect(m_clientChat->getChatHandler(), &ChatHandler::addRoomResult, this, &LobbyMainUI::handleRoomCreationResult);
    connect(m_clientChat->getChatHandler(), &ChatHandler::joinRoomResult, this, &LobbyMainUI::handleRoomJoinResult);
    connect(m_clientChat->getChatHandler(), &ChatHandler::leaveRoomResult, this, &LobbyMainUI::handleRoomLeaveResult);

    // 초기 채팅방 목록 요청
    requestRoomList();
}

LobbyMainUI::~LobbyMainUI()
{
    // UI 파일이 없으므로 'delete ui;'는 필요 없음
    // 자식 위젯들은 부모 위젯 (QSplitter, QStackedWidget 등)이 소멸될 때 함께 소멸
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
    obj["cmd"] = "list_r"; // 새로운 프로토콜 list_room
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 목록 요청 전송";
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
            m_roomListWidget->addItem(QString("%1 (%2명").arg(roomName).arg(memberCount));
        }
    }
}

// 채팅방 생성 버튼 클릭 시 (on_createChatRoomButton_clicked 구현)
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
        m_createRoomNameLineEdit->clear(); // 입력 필드 초기화
        requestRoomList(); // 방 목록 새로고침
    } else {
        QMessageBox::warning(this, "채팅방 생성 실패", message);
    }
}

// 방 입장 버튼 클릭 시
void LobbyMainUI::on_enterChatRoomButton_clicked()
{
    qDebug() << "채팅방 입장 버튼 클릭";

    // 선택된 방 이름 가져오기
    QListWidgetItem* selectedItem = m_roomListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "채팅방 입장", "입장할 채팅방을 선택해주세요.");
        return;
    }
    // "방이름 (N명)" 형식에서 방 이름만 추출
    QString fullText = selectedItem->text();
    QString roomName = fullText.left(fullText.indexOf(" ("));

    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "join_r"; // "joinRoom" 프로토콜 사용
    obj["rName"] = roomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 입장 요청 전송: " << doc.toJson(QJsonDocument::Compact);

    // 서버 응답에 따라 실제 채팅방으로 이동
    // m_currentChatRoomUI->setRoomName(roomName); // lobbyUI에 방 이름 설정 메서드 추가 필요
    // m_currentChatRoomUI->requestChatHistory(); // 채팅 기록 요청 (필요시)
}

// 방 입장 결과 처리 슬롯
void LobbyMainUI::handleRoomJoinResult(bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, "채팅방 입장", message);
        m_rightStackedWidget->setCurrentIndex(1); // 채팅방 상세 화면으로 전환
        m_enterChatRoomButton->setVisible(false); // 입장 버튼 숨김
        m_exitChatRoomButton->setVisible(true);   // 나가기 버튼 표시
        // m_currentChatRoomUI의 채팅 시작 등 추가 초기화 로직 호출 가능
    } else {
        QMessageBox::warning(this, "채팅방 입장 실패", message);
    }
}

// 방 나가기 버튼 클릭 시
void LobbyMainUI::on_exitChatRoomButton_clicked()
{
    qDebug() << "채팅방 나가기 버튼 클릭";
    // 현재 활성화된 채팅방 이름 (m_currentChatRoomUI가 가지고 있어야 함)
    // 예시: QString currentRoomName = m_currentChatRoomUI->getCurrentRoomName();
    // 임시로 그냥 방을 나간다고 가정 (실제로는 현재 입장한 방 이름 사용)
    QString currentRoomName = "임시방이름"; // 실제 입장한 방 이름으로 교체 필요

    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }

    QJsonObject obj;
    obj["cmd"] = "leave_r"; // "leaveRoom" 프로토콜
    obj["rName"] = currentRoomName;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "채팅방 나가기 요청 전송: " << doc.toJson(QJsonDocument::Compact);
}

// 방 나가기 결과 처리 슬롯
void LobbyMainUI::handleRoomLeaveResult(bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, "채팅방 나가기", message);
        m_rightStackedWidget->setCurrentIndex(0); // 채팅방 목록 위젯으로 전환
        m_enterChatRoomButton->setVisible(true);  // 입장 버튼 다시 표시
        m_exitChatRoomButton->setVisible(false);  // 나가기 버튼 숨김
        requestRoomList(); // 방 목록 새로고침
    } else {
        QMessageBox::warning(this, "채팅방 나가기 실패", message);
    }
}
