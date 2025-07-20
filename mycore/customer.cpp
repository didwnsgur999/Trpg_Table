#include "customer.h"

Customer::Customer() {}

bool Customer::searchProduct(QString name){
    if(m_myProduct.contains(name)){
        return true;
    }
    return false;
}
