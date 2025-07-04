#include "productUI.h"
#include "ui_productUI.h"
#include "product.h"
#include <QSharedPointer>
#include "backend.h"
#include <QJsonArray>

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
    int pCnt = ui->ProductCntEdit->text().toInt();
    newProduct->setId(pId);
    newProduct->setPrice(pPrice);
    newProduct->setName(pName);
    newProduct->setCnt(pCnt);

    QJsonObject obj;
    obj["cmd"]="add_p";
    obj["pName"] = pName;
    obj["pId"] = pId;
    obj["pPrice"] = pPrice;
    obj["pCnt"] = pCnt;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    //Backend::getInstance().addProduct(newProduct);
}

void ProductUI::on_goto_ui_clicked()
{
    emit requestPageChange(1);
}

