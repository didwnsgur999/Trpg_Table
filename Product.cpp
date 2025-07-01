#include "Product.h"

Product::Product() : m_id(0), m_name(""), m_price(0) {}

Product::Product(int id, QString name, int price) :m_id(id), m_name(name), m_price(price) {}
