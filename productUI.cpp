#include "productUI.h"
#include "ui_productUI.h"
#include "product.h"
#include <QSharedPointer>
#include "backend.h"

ProductUI::ProductUI(ClientChat* clientChat,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProductUI),m_clientChat(clientChat)
{
    ui->setupUi(this);
}

ProductUI::~ProductUI()
{
    delete ui;
}

void ProductUI::on_pushButton_clicked()
{
    QSharedPointer<Product> newProduct = QSharedPointer<Product>::create();
    QString pName = ui->ProductNameEdit->text();
    int pId = ui->ProductIdEdit->text().toInt();
    int pPrice = ui->ProductPriceEdit->text().toInt();

    newProduct->setId(pId);
    newProduct->setPrice(pPrice);
    newProduct->setName(pName);
    Backend::getInstance().addProduct(newProduct);
}

void ProductUI::on_goto_ui_clicked()
{
    emit requestPageChange(1);
}

