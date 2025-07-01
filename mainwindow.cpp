#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ProductUI.h"
#include <memory.h>

MainWindow::MainWindow(Backend* backend, QWidget *parent)
    : QMainWindow(parent), m_backend(backend)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // // 페이지 생성
    std::unique_ptr<ProductUI> _productUI = std::make_unique<ProductUI>(backend);
    //unique_ptr는 인자로 넘길때 move써야됨
    setProductUI(std::move(_productUI));

    // 스택에 추가
    ui->UI_STACK->addWidget(getProductUI());

    // 첫 화면은 PageA
    ui->UI_STACK->setCurrentIndex(0);

    qDebug() << "Stack count: " << ui->UI_STACK->count();
    qDebug() << "Current index: " << ui->UI_STACK->currentIndex();
    qDebug() << "Current widget: " << ui->UI_STACK->currentWidget();
    // //signal 연결
    // connect(page1, &Form::requestPageChange, this, [=](int index) {
    //     ui->change->setCurrentIndex(index);
    // });
    // connect(page2, &Form2::requestPageChange, this, [=](int index) {
    //     ui->change->setCurrentIndex(index);
    // });
}

MainWindow::~MainWindow()
{
    delete ui;
}
