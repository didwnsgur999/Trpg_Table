#include "storeui.h"
#include "ui_storeui.h"
#include "mycore/chathandler.h"
#include <QMessageBox>
#include "mycore/Backend.h"

storeUI::storeUI(ClientChat* clientChat,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::storeUI),m_clientChat(clientChat)
{
    ui->setupUi(this);
    connect(m_clientChat->getChatHandler(),&ChatHandler::productListReceived,this,&storeUI::loadProductList);
}

storeUI::~storeUI()
{
    delete ui;
}
void storeUI::loadProductList(const QJsonArray& array){
    //백엔드에 받은 제품값 넣기.
    Backend::getInstance().setProductFromJsonArr(array);

    const auto bprod=Backend::getInstance().getProducts();
    productlist.clear();
    ui->storeListWidget->clear();
    for(const auto& prod:bprod){
        productlist.insert(prod->getName(),prod);

        QListWidgetItem* item = new QListWidgetItem(QIcon(prod->getImage().scaled(80, 80, Qt::KeepAspectRatio)), prod->getName());
        item->setToolTip(QString("가격: %1원\n재고: %2개").arg(prod->getPrice()).arg(prod->getCnt()));

        ui->storeListWidget->addItem(item);
    }
}
void storeUI::loadUserProduct(){
    ui->userListWidget->clear();

    const auto& hash = Backend::getInstance().getUser()->getProd();
    for(auto it = hash.begin();it!=hash.end();++it){
        QString name = it.key();
        int id = it.value();
        QString displayText = QString("%1 (ID: %2)").arg(name).arg(id);
        ui->userListWidget->addItem(displayText);
    }
}

void storeUI::on_resetButton_clicked()
{
    //send list_p;
    if (!m_clientChat->isConnected()) {
        QMessageBox::warning(this, "오류", "서버에 연결되어 있지 않습니다.");
        return;
    }
    QJsonObject obj;
    obj["cmd"] = "list_p";
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}

void storeUI::on_storeListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString name = item->text();
    auto product = productlist.value(name);
    if (!product)
        return;

    int response = QMessageBox::question(
        this,
        "상품 구매",
        QString("'%1' 상품을 1개 구매하시겠습니까?\n가격: %2원\n남은 재고: %3개")
            .arg(name)
            .arg(product->getPrice())
            .arg(product->getCnt()),
        QMessageBox::Yes | QMessageBox::No
        );

    if (response == QMessageBox::Yes) {
        if (product->getCnt() <= 0) {
            QMessageBox::warning(this, "재고 없음", "해당 상품은 품절입니다.");
            return;
        }
        if (Backend::getInstance().getUser()->searchProduct(name)){
            QMessageBox::warning(this, "이미 있음", "해당 상품은 가지고 있습니다.");
            return;
        }
        //본인 product에 추가.
        Backend::getInstance().getUser()->addProd(name,product->getId());

        // 재고 감소
        product->setCnt(product->getCnt() - 1);

        //order주문 보내기.
        QJsonObject obj;
        obj["cmd"] = "add_o";
        obj["pname"]=name;
        obj["pcnt"]=1;
        QJsonDocument doc(obj);
        m_clientChat->sendData(doc);

        QMessageBox::information(this, "구매 완료",
                                 QString("%1 상품을 1개 구매했습니다.\n남은 재고: %2개")
                                     .arg(name)
                                     .arg(product->getCnt()));

        // UI에 툴팁 업데이트
        item->setToolTip(
            QString("가격: %1원\n재고: %2개")
                .arg(product->getPrice())
                .arg(product->getCnt())
            );
        loadUserProduct();
    }
}
void storeUI::resetStore(){
    on_resetButton_clicked();
    loadUserProduct();
}

