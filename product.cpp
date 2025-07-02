#include "product.h"

//for QSharedptr
Product::Product() : m_id(0), m_name(""), m_price(0) {}

Product::Product(int id, QString name, int price) :m_id(id), m_name(name), m_price(price) {}

QJsonObject Product::toJson() const {
    QJsonObject obj;
    obj["id"] = getId();
    obj["name"] = getName();
    obj["price"] = getPrice();
    return obj;
}

QSharedPointer<Product> Product::fromJson(const QJsonObject& obj) {
    return QSharedPointer<Product>::create(
        obj["id"].toInt(),
        obj["name"].toString(),
        obj["price"].toInt()
        );
}
