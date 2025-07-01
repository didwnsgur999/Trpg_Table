#include "Backend.h"
using namespace std;

Backend::Backend(QObject* parent) {}

void Backend::addProduct(QSharedPointer<Product> prod) {
    productList.push_back(prod);
}

const QVector<QSharedPointer<Product>>& Backend::getProducts() const {
    return productList;
}
