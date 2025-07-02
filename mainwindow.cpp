#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "productUI.h"
#include "lobbyui.h"
#include "product.h"
#include "Backend.h"
#include <memory.h>
#include <QDir>

MainWindow::MainWindow(Backend* backend, QWidget *parent)
    : QMainWindow(parent), m_backend(backend)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // UI 페이지 생성
    std::unique_ptr<ProductUI> _productUI = std::make_unique<ProductUI>(backend);
    std::unique_ptr<lobbyUI> _lobbyUI = std::make_unique<lobbyUI>(backend);
    //unique_ptr는 인자로 넘길때 move써야됨
    setProductUI(std::move(_productUI));
    setLobbyUI(std::move(_lobbyUI));

    // 스택에 추가 먼저 넣은 순서대로 0, 1, 2... 이다.
    ui->UI_STACK->addWidget(getProductUI());
    ui->UI_STACK->addWidget(getLobbyUI());

    // 첫 화면은 ProductUI
    ui->UI_STACK->setCurrentIndex(0);
    //signal 연결 -> 이게 이제 ui 변경 signal, slot
    connect(getProductUI(), &ProductUI::requestPageChange, this, [=](int index) {
        ui->UI_STACK->setCurrentIndex(index);
    });
    connect(getLobbyUI(), &lobbyUI::requestPageChange, this, [=](int index) {
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
    if(m_backend->saveJson("products.json", m_backend->getProducts())){
        qDebug() << "Current path:" << QDir::currentPath();
    }

    QVector<QSharedPointer<Product>> loadedProducts;
    if (m_backend->loadJson<Product>("products.json", loadedProducts,
                                     [](const QJsonObject& obj) { return Product::fromJson(obj); }))
    {
        for (const auto& prod : loadedProducts)
            qDebug() << prod->toJson();
    }
}

