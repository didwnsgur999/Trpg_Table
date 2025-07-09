#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "productUI.h"
// #include "lobbyui.h"     // 기존 lobbyUI는 이제 LobbyMainUI 안에서 사용되므로 여기서는 제거
#include "loginui.h"
#include "lobbymainui.h" // LobbyMainUI 포함
#include "product.h"
#include "Backend.h"
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_clientChat = new ClientChat(this); // ClientChat 단일 인스턴스 생성

    m_loginUI = new LoginUI(m_clientChat, this);      // 로그인 UI 생성
    m_productUI = new ProductUI(m_clientChat, this);  // 상품 관리 UI 생성
    m_lobbyMainUI = new LobbyMainUI(m_clientChat, this); // 로비 메인 UI 생성 (ClientChat 전달)

    // 스택 위젯에 UI 페이지들 추가 (순서 중요)
    ui->UI_STACK->addWidget(m_loginUI);      // 0번 인덱스: 로그인 UI (초기 화면)
    ui->UI_STACK->addWidget(m_lobbyMainUI); // 1번 인덱스: 로비 메인 UI (로그인 성공 시 이동)
    ui->UI_STACK->addWidget(m_productUI);    // 2번 인덱스: 상품 UI (관리자 기능)

    ui->UI_STACK->setCurrentIndex(0); // 첫 화면은 로그인 UI

    // UI 페이지 전환을 위한 시그널-슬롯 연결
    connect(m_loginUI, &LoginUI::requestPageChange, this, &MainWindow::changePage);
    connect(m_productUI, &ProductUI::requestPageChange, this, &MainWindow::changePage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePage(int index)
{
    ui->UI_STACK->setCurrentIndex(index);
    // LobbyMainUI로 전환될 때만 initializeLobby 호출
    if (index == 1) { // LobbyMainUI의 인덱스가 1이라고 가정
        m_lobbyMainUI->initializeLobby(); // <-- 추가: 로비 초기화 함수 호출
    }
}

// for debug - 이 부분은 기존 코드 유지
void MainWindow::on_ProductButton_clicked()
{
    if(Backend::getInstance().saveJson("products.json", Backend::getInstance().getProducts())){
        qDebug() << "Current path:" << QDir::currentPath();
    }

    QVector<QSharedPointer<Product>> loadedProducts;
    if (Backend::getInstance().loadJson<Product>("products.json", loadedProducts,
                                                 [](const QJsonObject& obj) { return Product::fromJson(obj); }))
    {
        for (const auto& prod : loadedProducts)
            qDebug() << prod->toJson();
    }
}
