#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "productUI.h"
#include "lobbyui.h"
#include "product.h"
#include "Backend.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //clientChat 단일로 MainWindow에서 관리.
    m_clientChat = new ClientChat(this);

    // UI 페이지 생성 + m_clientChat을 모두가 공유해야한다. UI에서 send보낼때 필요함.
    m_productUI = new ProductUI(m_clientChat,this);
    m_lobbyUI = new lobbyUI(m_clientChat,this);

    // 스택에 추가 먼저 넣은 순서대로 0, 1, 2... 이다.
    ui->UI_STACK->addWidget(m_productUI);
    ui->UI_STACK->addWidget(m_lobbyUI);

    // 첫 화면은 ProductUI
    ui->UI_STACK->setCurrentIndex(0);
    //signal 연결 -> 이게 이제 ui 변경 signal, slot
    connect(m_productUI, &ProductUI::requestPageChange, this, [=](int index) {
        ui->UI_STACK->setCurrentIndex(index);
    });
    connect(m_lobbyUI, &lobbyUI::requestPageChange, this, [=](int index) {
        ui->UI_STACK->setCurrentIndex(index);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

//for debug
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

