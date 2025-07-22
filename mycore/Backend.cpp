#include "Backend.h"

Backend::Backend(QObject* parent) {
    m_user = QSharedPointer<Customer>::create();
}
//product add
void Backend::addProduct(QSharedPointer<Product> prod) {
    m_productList.push_back(prod);
}

const QVector<QSharedPointer<Product>>& Backend::getProducts() const {
    return m_productList;
}

//room item get
const QVector<QSharedPointer<RoomItem>>& Backend::getRoomItems() const {
    return m_roomItemList;
}
void Backend::delRoomItem(int iid){
    for(int i=0; i<m_roomItemList.size(); i++){
        if(m_roomItemList[i]->iid==iid){
            m_roomItemList.removeAt(i);
            return;
        }
    }
    return;
}

/*
    QSharedPointer<Customer> m_user;
    QString m_roomname;
    QVector<QSharedPointer<Product>> m_productList;
    QVector<QSharedPointer<RoomItem>> m_roomItemList;
 */
QSharedPointer<RoomItem> Backend::searchRoomItem(int iid){
    for(auto roomitem:m_roomItemList){
        if(roomitem->iid==iid){
            return roomitem;
        }
    }
    return nullptr;
}
QSharedPointer<Product> Backend::searchProductId(int id){
    for(auto prod:m_productList){
        if(prod->getId()==id){
            return prod;
        }
    }
    return nullptr;
}



void Backend::setProductFromJsonArr(const QJsonArray& array){
    m_productList.clear();

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
        m_productList.push_back(newprod);
    }
}

void Backend::userInit(int id,QString name,QString pwd,const QJsonObject& prod){
    m_user->setName(name);
    m_user->setId(id);
    m_user->setPwd(pwd);
    for(auto it = prod.begin(); it != prod.end(); ++it){
        QString productName = it.key();
        int quantity = it.value().toInt();
        m_user->addProd(productName, quantity);
    }
    qDebug()<<name<<id<<pwd;
}

void Backend::setRoomItems(const QJsonArray& RoomItems){
    m_roomItemList.clear();
    for(const auto& item:RoomItems){
        QJsonObject obj = item.toObject();
        QSharedPointer<RoomItem> rItem= QSharedPointer<RoomItem>::create(RoomItem::fromJson(obj));
        qDebug()<<rItem->iid<<rItem->name;
        m_roomItemList.push_back(rItem);
    }
}

void Backend::clearRoomItem(){
    m_roomItemList.clear();
}
