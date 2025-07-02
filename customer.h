#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "Info.h"
#include <QString>
#include <QJsonObject>
#include <QSharedPointer>
class Customer : public Info{
public:
    Customer();
    Customer(int id, QString name, int price);

    //Product를 Json으로 변경하는 함수
    QJsonObject toJson() const ;
    //Json을 Product로 변경하는 함수.
    //외부에서 객체 없이 부르는 경우 사용해야 하므로 static이 붙는다.
    static QSharedPointer<Customer> fromJson(const QJsonObject& obj);

private:
    int m_id;
    QString m_name;
    int m_price;
};

#endif // CUSTOMER_H
