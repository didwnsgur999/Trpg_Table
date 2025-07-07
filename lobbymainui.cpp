#include "lobbymainui.h"
#include <QMessageBox> // QMessageBox 사용을 위해 추가
#include <QDebug>      // qDebug() 사용을 위해 추가

LobbyMainUI::LobbyMainUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , m_clientChat(clientChat)
{
    // 메인 Splitter 생성 (2:1 분할 담당)
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // --- 왼쪽 패널 (맵/상점) 설정 ---
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

    // --- 오른쪽 패널 (채팅) 설정 ---
    m_rightPaneWidget = new QWidget(m_mainSplitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rightPaneWidget);

    // 상단: 환영합니다! 및 채팅방 생성 버튼
    m_welcomeLabel = new QLabel("환영합니다!", m_rightPaneWidget); // 여기에 실제 사용자 이름을 받을 수 있도록 추후 수정
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_createChatRoomButton = new QPushButton("채팅방 생성", m_rightPaneWidget);
    connect(m_createChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_createChatRoomButton_clicked);

    rightLayout->addWidget(m_welcomeLabel);
    rightLayout->addWidget(m_createChatRoomButton);

    m_rightStackedWidget = new QStackedWidget(m_rightPaneWidget);

    // 채팅방 목록 위젯 (0번 인덱스)
    m_chatRoomListWidget = new QWidget(m_rightStackedWidget);
    QVBoxLayout* chatListLayout = new QVBoxLayout(m_chatRoomListWidget);
    QLabel* chatListLabel = new QLabel("~~채팅방 목록~~", m_chatRoomListWidget);
    chatListLabel->setAlignment(Qt::AlignCenter);
    chatListLayout->addWidget(chatListLabel);

    // 채팅방 입장 임시 버튼
    m_enterChatRoomButton = new QPushButton("채팅방 입장 (임시)", m_chatRoomListWidget);
    connect(m_enterChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_enterChatRoomButton_clicked);
    chatListLayout->addWidget(m_enterChatRoomButton);

    m_rightStackedWidget->addWidget(m_chatRoomListWidget); // 0번 인덱스 (채팅방 목록)

    // 기존 lobbyUI를 채팅방 상세 화면으로 활용 (1번 인덱스)
    // ClientChat 인스턴스를 lobbyUI에 전달합니다.
    m_currentChatRoomUI = new lobbyUI(m_clientChat, m_rightStackedWidget);
    m_rightStackedWidget->addWidget(m_currentChatRoomUI); // 1번 인덱스 (기존 lobbyUI)

    // 채팅방 나가기 버튼 (기존 lobbyUI 위에 추가, 임시)
    m_exitChatRoomButton = new QPushButton("채팅방 나가기", m_rightStackedWidget);
    m_exitChatRoomButton->setVisible(false); // 초기에는 숨김
    connect(m_exitChatRoomButton, &QPushButton::clicked, this, &LobbyMainUI::on_exitChatRoomButton_clicked);

    // lobbyUI 내부에 sendMesgEdit, textEdit, join_server_button 등이 있으므로
    // 해당 위젯들은 이 (m_currentChatRoomUI) 안에서 관리됩니다.
    // 기존 lobbyUI의 on_sendButton_clicked 등은 m_currentChatRoomUI 내에서 계속 작동합니다.

    rightLayout->addWidget(m_rightStackedWidget);
    m_rightPaneWidget->setLayout(rightLayout); // 레이아웃 설정

    // Splitter에 패널 추가 및 2:1 비율 설정
    m_mainSplitter->addWidget(m_leftPaneWidget);
    m_mainSplitter->addWidget(m_rightPaneWidget);

    // 초기 크기 비율 설정 (대략 2:1)
    // QWidget의 width가 아직 결정되지 않았을 수 있으므로,
    // 초기화 후 적절한 시점에 setSizes를 호출하거나,
    // 대략적인 값을 먼저 설정할 수 있움
    // 여기서는 기본값이거나 부모의 크기에 맞춰 자동 조절되도록 둘 수 있음
    // 예를 들어, sizes << 500 << 250; (픽셀 단위)
    QList<int> initialSizes;
    initialSizes << 500 << 250; // 초기 크기 비율 (예시)
    m_mainSplitter->setSizes(initialSizes);


    // LobbyMainUI의 메인 레이아웃에 Splitter 추가
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
    setLayout(mainLayout);
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

// 오른쪽 패널 슬롯
void LobbyMainUI::on_createChatRoomButton_clicked()
{
    QMessageBox::information(this, "알림", "채팅방 생성 기능은 추후 구현될 예정입니다!");
}

void LobbyMainUI::on_enterChatRoomButton_clicked()
{
    qDebug() << "채팅방 입장 버튼 클릭";
    m_rightStackedWidget->setCurrentIndex(1); // 기존 lobbyUI (채팅방) 위젯으로 전환
    m_enterChatRoomButton->setVisible(false); // 입장 버튼 숨김
    m_exitChatRoomButton->setVisible(true);   // 나가기 버튼 표시
    // 기존 lobbyUI의 서버 연결 로직을 이 시점에서 호출하도록 할 수도 있음
    // 예: m_currentChatRoomUI->on_join_server_clicked(); // IP/Port는 미리 설정되어 있어야 함
}

void LobbyMainUI::on_exitChatRoomButton_clicked()
{
    qDebug() << "채팅방 나가기 버튼 클릭";
    m_rightStackedWidget->setCurrentIndex(0); // 채팅방 목록 위젯으로 전환
    m_enterChatRoomButton->setVisible(true);  // 입장 버튼 다시 표시
    m_exitChatRoomButton->setVisible(false);  // 나가기 버튼 숨김
}
