#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginui.h"
#include "lobbymainui.h" // LobbyMainUI 포함
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
    m_lobbyMainUI = new LobbyMainUI(m_clientChat, this); // 로비 메인 UI 생성 (ClientChat 전달)

    // 스택 위젯에 UI 페이지들 추가 (순서 중요)
    ui->UI_STACK->addWidget(m_loginUI);      // 0번 인덱스: 로그인 UI (초기 화면)
    ui->UI_STACK->addWidget(m_lobbyMainUI);  // 1번 인덱스: 로비 메인 UI (로그인 성공 시 이동)

    ui->UI_STACK->setCurrentIndex(0); // 첫 화면은 로그인 UI

    // UI 페이지 전환을 위한 시그널-슬롯 연결
    connect(m_loginUI, &LoginUI::requestPageChange, this, &MainWindow::changePage);

    // 로그인 성공 시 로비 메인 UI로 전환하고 초기화 함수 호출
    connect(m_loginUI, &LoginUI::loginSuccess, m_lobbyMainUI, &LobbyMainUI::initializeLobby);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePage(int index)
{
    this->setWindowTitle(QString(tr("현재 접속자: %1").arg(Backend::getInstance().getUser()->getName())));
    ui->UI_STACK->setCurrentIndex(index);
}
