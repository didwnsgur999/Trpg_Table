#include "ProductUI.h"
#include "ui_ProductUI.h"
#include "Product.h"
#include <QSharedPointer>
#include "backend.h"

ProductUI::ProductUI(Backend* backend,QWidget *parent)
    : QWidget(parent),m_backend(backend)
    , ui(new Ui::ProductUI)
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
    m_backend->addProduct(newProduct);
}

