#include "storeui.h"
#include "ui_storeui.h"
#include "chathandler.h"
#include <QMessageBox>

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
    ui->storeListWidget->clear();
    productlist.clear();

    for(const auto& prod:array){
        QJsonObject obj = prod.toObject();
        int id = obj["pId"].toInt();
        QString name = obj["pName"].toString();
        int price = obj["pPrice"].toInt();
        int cnt = obj["pCnt"].toInt();
        auto newprod = QSharedPointer<Product>::create(id,name,price,cnt);
        QString base64Image = obj["pImage"].toString();
        QByteArray imageBytes = QByteArray::fromBase64(base64Image.toUtf8());
        QPixmap pixmap;
        pixmap.loadFromData(imageBytes, "PNG");
        newprod->setImage(pixmap);
        productlist.insert(name,newprod);

        QListWidgetItem* item = new QListWidgetItem(QIcon(pixmap.scaled(80, 80, Qt::KeepAspectRatio)), name);
        item->setToolTip(QString("가격: %1원\n재고: %2개").arg(price).arg(cnt));

        ui->storeListWidget->addItem(item);
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
    }
}

