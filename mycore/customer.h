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

    //customer를 Json으로 변경하는 함수
    QJsonObject toJson() const ;
    //Json을 customer로 변경하는 함수.
    //외부에서 객체 없이 부르는 경우 사용해야 하므로 static이 붙는다.
    static QSharedPointer<Customer> fromJson(const QJsonObject& obj);

    //Getter Setter
    int getId() const { return m_id; }
    void setId(int id) { m_id=id;}
    QString getName() const { return m_name; }
    void setName(QString name) { m_name=name;}
    QString getPwd() const { return m_pwd; }
    void setPwd(QString pwd) { m_pwd=pwd;}
    const QHash<QString,int>& getProd(){ return m_myProduct;}
    void addProd(QString pname,int pid){ m_myProduct.insert(pname,pid);}
    bool searchProduct(QString name);

private:
    int m_id;
    QString m_name;
    QString m_pwd;
    //제품 가지고 있어야됨.
    QHash<QString, int> m_myProduct;
};

#endif // CUSTOMER_H
