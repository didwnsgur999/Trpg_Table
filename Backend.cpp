#include "Backend.h"

Backend::Backend(QObject* parent) {
    user = QSharedPointer<Customer>::create();
}

void Backend::addProduct(QSharedPointer<Product> prod) {
    productList.push_back(prod);
}

const QVector<QSharedPointer<Product>>& Backend::getProducts() const {
    return productList;
}

void Backend::userInit(int id,QString name,QString pwd,const QJsonObject& prod){
    user->setName(name);
    user->setId(id);
    user->setPwd(pwd);
    for(auto it = prod.begin(); it != prod.end(); ++it){
        QString productName = it.key();
        int quantity = it.value().toInt();
        user->addProd(productName, quantity);
    }
}
